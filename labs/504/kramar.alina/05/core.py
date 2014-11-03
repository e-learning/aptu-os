#!/usr/bin/env python3

import sys
import os
import math

from descriptors import BlockDescriptor, DirectoryDescriptor, normalize_name

"""
block size >= 1024B
name length <= 10
max size 50MB
max block count 51200

_____________________

M
root
files data
_____________________

block descriptor(uint16_t)
  next block
  ffff - not used
  fffe - last block

directory descriptor row(13 bytes)
  char[10] name
  byte is_directory | is_deleted | is_last | is_file
  uint16_t first_block
"""

class File(object):
    is_file = True
    is_dir = False

    def __init__(self, fs, name, first_block, length):
        self.fs = fs
        self.name = name
        self.first_block = first_block
        self.length = length

    def read(self):
        chunks = []
        to_read = self.length
        block = self.first_block
        while to_read > 0:
            r = min(to_read, self.fs.block_size)
            to_read -= r
            chunks.append(self.fs.read_from(block, 0, r))
            block = self.fs.next_block(block)
        assert block is None
        return b''.join(chunks)

    def ls(self):
        size_in_blocks = math.ceil(self.length / self.fs.block_size)
        return [(self.name.decode(), size_in_blocks)]

    def rm(self):
        self.fs.rm_blocks(self.first_block)


class Directory(object):
    is_file = False
    is_dir = True

    def __init__(self, fs, first_block):
        self.fs = fs
        self.descriptor_map = {}
        self.descriptor_locs = {}
        self.first_block = first_block
        for block, idx, dd in self.iter_descriptors():
            if not dd.is_deleted and not dd.is_last:
                self.descriptor_map[dd.name] = dd
                self.descriptor_locs[dd.name] = (block, idx)

    def has_child(self, name):
        return name in self.descriptor_map

    def visit(self, name):
        x = self.descriptor_map[name]
        if x.is_dir:
            return Directory(self.fs, x.first_block)
        else:
            return File(self.fs, name, x.first_block, x.length)

    def visit_all(self):
        for k in self.descriptor_map:
            yield k, self.visit(k)

    def attach_descriptor(self, desc):
        self.descriptor_map[desc.name] = desc
        for dd_block, dd_i, dd in self.iter_descriptors():
            if dd.is_deleted or dd.is_last:
                self.fs.write_dir_descriptor(dd_block, dd_i, desc)
                if dd.is_last:
                    if dd_i + 1 < self.fs.directory_descriptors_per_block:
                        self.fs.write_dir_descriptor(
                            dd_block, dd_i + 1, DirectoryDescriptor.LAST
                        )
                    else:
                        block = fs.alloc_block(index)
                        self.fs.write_dir_descriptor(
                            block, 0, DirectoryDescriptor.LAST
                        )
                return
        assert False

    def detach_descriptor(self, name):
        d = self.descriptor_map[name]
        block, idx = self.descriptor_locs[name]
        self.fs.write_dir_descriptor(
            block, idx, DirectoryDescriptor.DELETED
        )
        return d

    def ls(self):
        return [(name.decode(), 'd' if d.is_dir else 'f') for name, d
                in sorted(self.descriptor_map.items(), key=lambda x: x[0])]

    def rm(self):
        for name in self.descriptor_map:
            self.visit(name).rm()
        self.fs.rm_blocks(self.first_block)

    def iter_descriptors(self):
        current_block = self.first_block
        while current_block:
            for i in range(self.fs.directory_descriptors_per_block):
                data = self.fs.read_from(current_block, i * DirectoryDescriptor.size, DirectoryDescriptor.size)
                dd = DirectoryDescriptor.from_data(data)
                yield current_block, i, dd
                if dd.is_last:
                    break
            current_block = self.fs.next_block(current_block)

class FS(object):
    def __init__(self, root):
        self.root = root
        with open(os.path.join(self.root, 'config')) as config:
            data = config.read()

        self.block_size, self.n_blocks = [int(x) for x in data.split()]

    def init_disk(self):
        for i in range(self.n_blocks):
            with open(self.block_name(i), "wb") as f:
                f.write(b'\0' * self.block_size)

    def format_fs(self):
        assert self.n_blocks > self.table_length
        for i in range(self.n_blocks):
            table_block = i // self.block_descriptors_per_block
            offset = (i % self.block_descriptors_per_block) * BlockDescriptor.size
            descriptor = BlockDescriptor.NOT_USED if i > self.table_length else BlockDescriptor.LAST
            self.write_to(table_block, offset, descriptor.data)
        self.write_to(self.root_dir, 0, DirectoryDescriptor.LAST.data)

    def mkdir(self, path):
        current_dir = Directory(self, self.root_dir)
        for segment in self._split_path(path):
            if not current_dir.has_child(segment):
                dd = self._make_dir_descriptor(segment)
                current_dir.attach_descriptor(dd)
            current_dir = current_dir.visit(segment)
            assert current_dir.is_dir

    def import_file(self, path, data):
        *path, basename = self._split_path(path)
        d = self._cd(path)
        assert not d.has_child(basename)
        fd = self._make_file_descriptor(basename, data)
        d.attach_descriptor(fd)

    def export_file(self, path):
        *path, basename = self._split_path(path)
        d = self._cd(path)
        f = d.visit(basename)
        assert f.is_file
        return f.read()

    def ls(self, path):
        return self._cd(self._split_path(path)).ls()

    def move(self, src, trgt):
        *spath, sbasename = self._split_path(src)
        *tpath, tbasename = self._split_path(trgt)
        s = self._cd(spath)
        t = self._cd(tpath)

        desc = s.detach_descriptor(sbasename)
        desc.name = tbasename
        t.attach_descriptor(desc)

    def rm(self, path):
        *path, basename = self._split_path(path)
        d = self._cd(path)
        assert d.is_dir
        d.visit(basename).rm()
        d.detach_descriptor(basename)

    def copy(self, src, trgt):
        s = self._cd(self._split_path(src))
        *t, basename = self._split_path(trgt)
        t = self._cd(t)
        self._copy(s, t, basename)

    def _copy(self, src_desc, trgt_desc, basename):
        if src_desc.is_file:
            d = self._make_file_descriptor(basename, src_desc.read())
        else:
            d = self._make_dir_descriptor(basename)

        trgt_desc.attach_descriptor(d)
        if src_desc.is_dir:
            v = trgt_desc.visit(basename)
            for name, desc in src_desc.visit_all():
                self._copy(desc, v, name)

    @property
    def block_descriptors_per_block(self):
        return self.block_size // BlockDescriptor.size

    @property
    def directory_descriptors_per_block(self):
        return self.block_size // DirectoryDescriptor.size

    @property
    def root_dir(self):
        return self.table_length

    @property
    def table_length(self):
        return math.ceil(self.n_blocks / self.block_descriptors_per_block)

    def block_name(self, index):
        return os.path.join(self.root, "block{}".format(index))

    def block_descriptor_address(self, block):
        table_block = block // self.block_descriptors_per_block
        table_offset = (block % self.block_descriptors_per_block) * BlockDescriptor.size
        return table_block, table_offset

    def read_from(self, index, offset, count):
        with open(self.block_name(index), "rb") as block:
            block.seek(offset)
            return block.read(count)

    def write_to(self, index, offset, data):
        assert offset >= 0
        assert offset + len(data) <= self.block_size, \
            "Block size is {}. Attempt to write at {}".format(
                self.block_size, offset + len(data))
        size = os.path.getsize(self.block_name(index))
        assert size == self.block_size, \
            "Block size expected {}, got {}".format(self.block_size, size)
        with open(self.block_name(index), "r+b") as block:
            block.seek(offset)
            block.write(data)

    def read_block_descriptor(self, block):
        index, offset = self.block_descriptor_address(block)
        data = self.read_from(index, offset, BlockDescriptor.size)
        return BlockDescriptor.from_data(data)

    def write_block_descriptor(self, block, descr):
        index, offset = self.block_descriptor_address(block)
        self.write_to(index, offset, descr.data)

    def write_dir_descriptor(self, block, i, descr):
        offset = i * DirectoryDescriptor.size
        self.write_to(block, offset, descr.data)

    def next_block(self, block):
        descr = self.read_block_descriptor(block)
        assert not descr.is_not_used
        if descr.is_last:
            return None
        return descr.value

    def alloc_block(self, parent=None):
        if parent:
            parent_descr = self.read_block_descriptor(parent)
            assert parent_descr.is_last
        for i, block in self.iter_blocks():
            if block.is_not_used:
                self.write_block_descriptor(i, BlockDescriptor.LAST)
                if parent:
                    self.write_block_descriptor(parent, BlockDescriptor(i))
                return i
        assert False

    def iter_blocks(self):
        for i in range(self.n_blocks):
            block = self.read_block_descriptor(i)
            yield i, block

    def rm_blocks(self, block):
        while block:
            n = self.next_block(block)
            self.write_block_descriptor(block, BlockDescriptor.NOT_USED)
            block = n

    def _make_file_descriptor(self, name, f):
        file_blocks = []
        blocks_count = math.ceil(len(f) / self.block_size)
        for i, block in self.iter_blocks():
            if block.is_not_used:
                file_blocks.append(i)
            if len(file_blocks) == blocks_count:
                break
        else:
            assert False

        for i, k in enumerate(file_blocks):
            self.write_to(k, 0, f[self.block_size * i:self.block_size * (i + 1)])
        for p, q in zip(file_blocks, file_blocks[1:]):
            self.write_block_descriptor(p, BlockDescriptor(q))
        self.write_block_descriptor(file_blocks[-1], BlockDescriptor.LAST)

        return DirectoryDescriptor(
            name,
            DirectoryDescriptor.FILE_FLAG,
            file_blocks[0],
            len(f)
        )

    def _make_dir_descriptor(self, name):
        new_dir_first_block = self.alloc_block()
        self.write_dir_descriptor(new_dir_first_block, 0, DirectoryDescriptor.LAST)
        return DirectoryDescriptor(name, DirectoryDescriptor.DIRECTORY_FLAG, new_dir_first_block)

    def _split_path(self, path):
        assert path[0] == '/'
        return [normalize_name(seg) for seg in path.split('/') if seg]

    def _cd(self, path):
        current_dir = Directory(self, self.root_dir)
        for segment in path:
            if not current_dir.has_child(segment):
                assert False
            current_dir = current_dir.visit(segment)
        return current_dir


def main():
    _, command, root, *params = sys.argv
    fs = FS(root)
    def import_file(src, trgt):
        with open(src, 'rb') as f:
            data = f.read()
        fs.import_file(trgt, data)

    def export_file(src, trgt):
        data = fs.export_file(src)
        with open(trgt, 'wb') as f:
            f.write(data)

    def ls(path):
        for (entry, t) in fs.ls(path):
            print(entry, t)

    {
        'init': fs.init_disk,
        'format': fs.format_fs,
        'import': import_file,
        'export' : export_file,
        'ls' : ls,
        'copy' : fs.copy,
        'rm' : fs.rm,
        'move' : fs.move,
        'mkdir' : fs.mkdir,
    }[command](*params)

if __name__ == "__main__":
    main()
