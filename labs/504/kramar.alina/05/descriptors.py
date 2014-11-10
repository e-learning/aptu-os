import struct

class DescriptorMeta(type):
     def __init__(cls, name, bases, nmspc):
        super(DescriptorMeta, cls).__init__(name, bases, nmspc)

        if cls.FIELDS is not None:
            fields = cls.FIELDS.strip().splitlines()
            names, specs = zip(*[x.split() for x in fields])

            cls._fields = names
            cls._struct = struct.Struct(">" + ' '.join(specs))
            cls.size = cls._struct.size

class Descriptor(object, metaclass=DescriptorMeta):
    FIELDS = None

    @property
    def data(self):
        fields = [getattr(self, a) for a in self._fields]
        return self._struct.pack(*fields)

    @classmethod
    def from_data(cls, data):
        return cls(*cls._struct.unpack(data))

    def __eq__(self, other):
        assert isinstance(other, self.__class__)
        return all(getattr(self, a) == getattr(other, a) for a in self._fields)

    def __hash__(self):
        return hash(tuple(getattr(self, a) for a in self._fields))

def normalize_name(name):
    name = name if isinstance(name, bytes) else name.encode('ascii')
    assert len(name) <= 10
    name += b'\0' * (10 - len(name))
    return name

class DirectoryDescriptor(Descriptor):
    FIELDS = """
        name        10s
        flags       B
        first_block H
        length      I
    """
    LAST_FLAG      = 0
    DELETED_FLAG   = 1
    DIRECTORY_FLAG = 2
    FILE_FLAG      = 3

    def __init__(self, name, flags, first_block=0, length=0):
        self.name = normalize_name(name)
        self.flags = flags
        self.first_block = first_block
        self.length = length

    @property
    def is_last(self):
        return self == DirectoryDescriptor.LAST

    @property
    def is_deleted(self):
        return self == DirectoryDescriptor.DELETED

    @property
    def is_dir(self):
        return self.flags == DirectoryDescriptor.DIRECTORY_FLAG

    @property
    def is_file(self):
        return self.flags == DirectoryDescriptor.FILE_FLAG


DirectoryDescriptor.LAST = DirectoryDescriptor('_last', DirectoryDescriptor.LAST_FLAG)
DirectoryDescriptor.DELETED = DirectoryDescriptor('_deleted', DirectoryDescriptor.DELETED_FLAG)

class BlockDescriptor(Descriptor):
    FIELDS = "value H"

    def __init__(self, value):
        self.value = value

    @property
    def is_last(self):
        return self == BlockDescriptor.LAST

    @property
    def is_not_used(self):
        return self == BlockDescriptor.NOT_USED

BlockDescriptor.NOT_USED = BlockDescriptor(0xffff)
BlockDescriptor.LAST = BlockDescriptor(0xfffe)
