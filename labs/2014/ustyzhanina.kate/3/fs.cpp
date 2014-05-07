#include "fs.h"

void FileSystem::write_meta()
{
	for(size_t i = 0; i != meta_blocks_count; ++i)
	{
		vector<char> buffer;

		//if there is free blocks write first as 1, else write 0
		if (i == 0)
			free_blocks_count > 0 ? meta_info[0] = 1 : meta_info[0] = 0;

		for (size_t k = size * i; k != size * (i + 1); ++k)
			buffer.push_back(meta_info[k]);

		ofstream file(utils.path_append(root_dirname, to_string(i)));
		file.write(buffer.data(), size);
	}
}

//read config file
void FileSystem::read_config()
{
	string path = root_dirname + "/";
	ifstream f(utils.path_append(path, "config").c_str());

	if(f.is_open())
		f >> size >> all_blocks_count;

	meta_blocks_count = (size_t)all_blocks_count / size + 1;
	free_blocks_count = all_blocks_count - meta_blocks_count;
	meta_info.assign(size * meta_blocks_count, 0);
	main_info_size = all_blocks_count + 1;
}

//blocks count
void FileSystem::init()
{
	read_config();
	vector<char> buffer(size, 0);

	for (size_t i = 0; i != all_blocks_count; ++i)
	{
		ofstream file(utils.path_append(root_dirname, to_string(i)));
		file.write(buffer.data(), size);
	}

	write_meta();
}

void FileSystem::fix_free_blocks()
{
	if (meta_info[0] == 0)
		free_blocks_count = 0;
	else
		for(size_t i = 1; i != meta_info.size(); ++i)
			if (meta_info[i] == 1)
				free_blocks_count -= 1;
}

void FileSystem::read_meta_data()
{
	char * temp = new char[size];
	meta_info.clear();

	for(size_t i = 0; i != meta_blocks_count; ++i)
	{
		ifstream in(utils.path_append(root_dirname, to_string(i)));
		in.read(temp, size);

		for(size_t k = 0; k != size; ++k)
			meta_info.push_back((size_t)temp[k]);
	}

	fix_free_blocks();
}


void FileSystem::write_directory(DirectoryD dir, size_t first_block)
{
	int temp = dir.get_size() / size + 1;
	int rest =  temp * size -  dir.get_size();
	size_t need_blocks = temp ;
	rest < temp * sizeof(size_t) ? need_blocks += (temp * sizeof(size_t) - rest) / size + 2 : need_blocks += 1;

	if(need_blocks > free_blocks_count)
		throw runtime_error("not enough memory");

	queue <pair<string, size_t> > need_write_data = dir.files;
	size_t current_block = alloc_block(first_block);
	--need_blocks;
	size_t next_empty = 0;

	if (need_write_data.size() != 0)
		next_empty = find_first_empty();

	ofstream file(utils.path_append(root_dirname, to_string(current_block)));
	size_t entr_count = dir.files.size();
	file.write((char *)&DIR, sizeof(size_t));
	file.write((char *)&next_empty, sizeof(size_t));
	file.write((char *)&entr_count, sizeof(size_t));
	utils.write_string(file, dir.name);
	file.write((char *)&dir.time, sizeof(time_t));
	size_t free_size = size - sizeof(size_t);
	size_t entry_size = 10 + sizeof(size_t);
	size_t count_may_write = free_size / entry_size;
	size_t header_size = sizeof(size_t) * 3 + sizeof(time_t) + 10;
	fill_free_file(file, size - header_size);

	while(need_blocks > 0 && need_write_data.size() != 0)
	{
		current_block = alloc_block();
		--need_blocks;
		next_empty = 0;

		if (need_blocks != 0)
			next_empty = find_first_empty();

		ofstream f_i(utils.path_append(root_dirname, to_string(current_block)));
		f_i.write((char *)&next_empty, sizeof(size_t));
		size_t size_write = count_may_write > need_write_data.size() ? need_write_data.size() : count_may_write;

		for (int i = 0; i != size_write; ++i)
		{
			pair <string, size_t> m_pair = need_write_data.front();
			need_write_data.pop();
			utils.write_string(f_i, m_pair.first);
			f_i.write((char *)&m_pair.second, sizeof(size_t));
		}

		fill_free_file(f_i, size - size_write * entry_size - sizeof(size_t));
	}
}

DirectoryD FileSystem::read_directory(size_t block)
{
	vector <size_t> blocks;
	ifstream in(utils.path_append(root_dirname, to_string(block)));
	size_t dir_m, next_block, entr_count, entr_need_read;
	blocks.push_back(block);
	in.read((char *)&dir_m, sizeof(size_t));
	in.read((char *)&next_block, sizeof(size_t));

	if (next_block != 0)
	{
		blocks.push_back(next_block);
	}

	in.read((char *)&entr_count, sizeof(size_t));
	string dir_name = utils.read_string(in);
	time_t time;
	in.read((char *)&time, sizeof(time_t));
	entr_need_read = entr_count;
	size_t free_size = size - sizeof(size_t);
	size_t entry_size = 10 + sizeof(size_t);
	size_t count_may_read = free_size / entry_size;
	queue<pair<string, size_t> > m_queue;

	while (next_block != 0)
	{
		ifstream in_i(utils.path_append(root_dirname, to_string(next_block)));
		in_i.read((char *)&next_block, sizeof(size_t));

		if (next_block != 0)
		{
			blocks.push_back(next_block);
		}

		size_t size_read = count_may_read > entr_need_read ? entr_need_read : count_may_read;

		for(size_t i = 0; i !=  size_read; ++i)
		{
			string m_name = utils.read_string(in_i);
			size_t first_block;
			in_i.read((char *)&first_block, sizeof(size_t));
			m_queue.push(make_pair(m_name, first_block));
		}

		entr_need_read -= size_read;
	}

	DirectoryD dir(dir_name, time, 0, m_queue);
	dir.blocks  = blocks;
	return dir;
}
//write out file
size_t FileSystem::write_out_file_to_blocks(FileD file_m, string in_path)
{
	ifstream in(in_path);

	if (!in)
		throw runtime_error("can't read file " + in_path);

	size_t data_size = utils.get_file_size(in_path.c_str());
	int temp = data_size / size + 1;
	int rest =  temp * size - data_size;
	size_t need_blocks = temp;
	rest < temp * sizeof(size_t) ? need_blocks += (temp * sizeof(size_t) - rest) / size + 2 : need_blocks += 1;

	if(need_blocks > free_blocks_count)
		throw runtime_error("not enough memory");

	size_t current_block = alloc_block();
	size_t rez = current_block;
	--need_blocks;
	size_t next_empty = 0;

	if (data_size != 0)
		next_empty = find_first_empty();

	ofstream file(utils.path_append(root_dirname, to_string(current_block)));
	file.write((char *)&FILE, sizeof(size_t));
	utils.write_string(file, file_m.name);
	file.write((char *)&file_m.time, sizeof(time_t));
	file.write((char *)&data_size, sizeof(size_t));
	file.write((char *)&next_empty, sizeof(size_t));
	size_t buf_size = size - sizeof(size_t);
	char * buf = new char[buf_size];
	size_t header_size = 3 * sizeof(size_t) + 10 + sizeof(time_t);
	fill_free_file(file, size - header_size);

	while (need_blocks > 0)
	{
		current_block = alloc_block();
		--need_blocks;
		next_empty = 0;

		if (need_blocks != 0)
			next_empty = find_first_empty();

		ofstream f_i(utils.path_append(root_dirname, to_string(current_block)));
		f_i.write((char *)&next_empty, sizeof(size_t));
		in.read(buf, buf_size);
		f_i.write(buf, buf_size);
		fill_free_file(f_i, size - sizeof(size_t) - buf_size);
	}

	return rez;
}

//write out file
size_t FileSystem::write_file_to_blocks(FileD file_m)
{
	size_t need_blocks = file_m.blocks.size();

	if(need_blocks > free_blocks_count)
		throw runtime_error("not enough memory");

	size_t current_block = alloc_block();
	size_t first_block = current_block;
	--need_blocks;
	size_t next_empty = 0;

	if (need_blocks > 1)
		next_empty = find_first_empty();

	ofstream file(utils.path_append(root_dirname, to_string(current_block)));
	file.write((char *)&FILE, sizeof(size_t));
	utils.write_string(file, file_m.name);
	file.write((char *)&file_m.time, sizeof(time_t));
	file.write((char *)&file_m.data_size, sizeof(size_t));
	file.write((char *)&next_empty, sizeof(size_t));
	fill_free_file(file, size - sizeof(size_t) * 3 - sizeof(time_t)  - 10);

	for (int i = 0; i != file_m.data.size(); ++i)
	{
		current_block = alloc_block();
		--need_blocks;
		next_empty = 0;

		if (need_blocks != 0)
			next_empty = find_first_empty();

		ofstream f_i(utils.path_append(root_dirname, to_string(current_block)));
		f_i.write((char *)&next_empty, sizeof(size_t));
		f_i.write(file_m.data[i].first, file_m.data[i].second);
		fill_free_file(file, size - sizeof(size_t) - file_m.data[i].second);
	}

	return first_block;
}


void FileSystem::fill_free_file(ofstream  & out, size_t need_size)
{
	vector <char> buffer(need_size, 0);
	out.write(buffer.data(), need_size);
}



void FileSystem::fill_free_block(vector <size_t> blocks)
{
	for (int i = 0; i != blocks.size(); ++i)
	{
		meta_info[blocks[i]] = 0;
		ofstream file(utils.path_append(root_dirname, to_string(blocks[i])));
		fill_free_file(file, size);
	}
}

FileD FileSystem::read_file_from_blocks(size_t first_block)
{
	vector <size_t> blocks;
	blocks.push_back(first_block);
	ifstream file(utils.path_append(root_dirname, to_string(first_block)));
	size_t m_file, data_size, next_block;
	time_t time;
	file.read((char *)&m_file, sizeof(size_t));
	string file_name = utils.read_string(file);
	file.read((char *)&time, sizeof(time_t));
	file.read((char *)&data_size, sizeof(size_t));
	size_t rez_size = data_size;
	file.read((char *)&next_block, sizeof(size_t));

	if(next_block != 0)
	{
		blocks.push_back(next_block);
	}

	size_t buf_size = size - sizeof(size_t);
	vector <pair<char *, size_t> > my_data;

	while (next_block != 0)
	{
		ifstream in_i(utils.path_append(root_dirname, to_string(next_block)));
		in_i.read((char *)&next_block, sizeof(size_t));

		if(next_block != 0)
		{
			blocks.push_back(next_block);
		}

		next_block != 0 ? buf_size = size - sizeof(size_t) : buf_size = data_size;
		char * buf  = new char[buf_size];
		in_i.read(buf, buf_size);
		my_data.push_back(make_pair(buf, buf_size));
		data_size -= buf_size;
	}

	FileD d_file(file_name, time);
	d_file.blocks = blocks;
	d_file.data = my_data;
	d_file.data_size = rez_size;
	return d_file;
}

void FileSystem::format()
{
	init();
	string name = "/";
	queue <pair<string, size_t> > list;
	size_t dir_num = find_first_empty();
	DirectoryD dir(name, time(0), dir_num, list);
	write_directory(dir, dir_num);
	write_meta();
}


pair<string, size_t> FileSystem:: find_directory(string instr, bool up_to_end)
{
	vector <string> parts  = utils.split(instr, '/');

	if(parts.size() < 1 || parts[0] != "")
		throw runtime_error("Path" + instr + " should start with / - root directory");

	//root block
	size_t block_num = meta_blocks_count;
	string name = "/";

	if (parts.size() == 2 && !up_to_end)
	{
		return make_pair(name, block_num);
	}

	size_t parts_size = parts.size() - 1;
	size_t begin = 1, compare_ind;

	if (up_to_end)
	{
		begin  = 0;
	}

	for(int k = begin; k != parts_size; ++k)
	{
		if (up_to_end)
			compare_ind = k + 1;
		else
			compare_ind = k;

		bool find = false;
		DirectoryD dir = read_directory(block_num);
		queue<pair<string, size_t> > temp = dir.files;
		size_t list_size = temp.size();

		for(int i = 0; i != list_size; ++i)
		{
			if(temp.front().first == parts[compare_ind])
			{
				block_num = temp.front().second;
				name = temp.front().first;
				find  = true;
				break;
			}

			temp.pop();
		}

		if (!find)
			throw runtime_error("wrong path " + instr);
	}

	return make_pair(name, block_num);
}
bool FileSystem::check_exist_file(string in_file, queue<pair<string, size_t> > cur_list)
{
	size_t list_size  = cur_list.size();

	for (size_t i = 0 ; i != list_size; ++i)
	{
		if (in_file == cur_list.front().first)
			return true;

		cur_list.pop();
	}

	return false;
}

void FileSystem::import(string from, string to)
{
	read_config();
	read_meta_data();
	vector <string> to_path = utils.split(to, '/');
	DirectoryD out_directory = read_directory(find_directory(to, false).second);

	if (check_exist_file(to_path[to_path.size() - 1], out_directory.files))
		throw runtime_error("this file " + to + " already exist in filesystem");

	//write file
	FileD m_file(to_path[to_path.size() - 1], time(0));
	size_t first_file_block = write_out_file_to_blocks(m_file, from);
	out_directory.files.push(make_pair(m_file.name, first_file_block));
	out_directory.time = time(0);
	fill_free_block(out_directory.blocks);
	write_directory(out_directory, out_directory.blocks[0]);
	write_meta();
}

void FileSystem::modify_file(string from, string to, bool fl)
{
	read_config();
	read_meta_data();
	DirectoryD from_directory = read_directory(find_directory(from, false).second);
	vector <string> from_path = utils.split(from, '/');
	string to_name = from_path[from_path.size() - 1];
	queue <pair<string, size_t> > temp = from_directory.files;
	queue <pair<string, size_t> > new_list;
	size_t list_size = temp.size();
	bool find = false;

	for(int i = 0; i != list_size; ++i)
	{
		if(temp.front().first == to_name)
		{
			find = true;
			FileD cur_file = read_file_from_blocks(temp.front().second);

			if (fl)
			{
				ofstream out_f(to);

				for(size_t k = 0; k != cur_file.data.size(); ++k)
					out_f.write(cur_file.data[k].first, cur_file.data[k].second);
			}

			fill_free_block(cur_file.blocks);
			temp.pop();
		}
		else
		{
			new_list.push(temp.front());
			temp.pop();
		}
	}

	if (!find)
		throw runtime_error("no such file " + from);

	from_directory.files = new_list;
	from_directory.time = time(0);
	fill_free_block(from_directory.blocks);
	write_directory(from_directory, from_directory.blocks[0]);
	write_meta();
}

void FileSystem::export_(string from, string to)
{
	modify_file(from, to, true);
}

void FileSystem::rm_file(string const from)
{
	modify_file(from, "" , false);
}

void FileSystem::move(string  const source, string const to)
{
	read_config();
	read_meta_data();
	DirectoryD to_dir = read_directory(find_directory(to, true).second);
	pair<string, size_t> new_elem = find_directory(source, true);
	to_dir.files.push(new_elem);
	fill_free_block(to_dir.blocks);
	to_dir.time = time(0);
	write_directory(to_dir, to_dir.blocks[0]);
	rm_file(source);
	write_meta();
}

void FileSystem::copy_directory(DirectoryD source, DirectoryD to)
{
	size_t list_size = source.files.size();
	queue <pair<string, size_t> > list = source.files;

	for (size_t i = 0; i != list_size; ++i)
	{
		ifstream in(utils.path_append(root_dirname, to_string(list.front().second)));
		size_t ident;
		in.read((char *)&ident, sizeof(size_t));
		in.close();

		if (ident == FILE)
		{
			FileD cur_file = read_file_from_blocks(list.front().second);
			size_t new_block = write_file_to_blocks(cur_file);
			to.files.push(make_pair(list.front().first, new_block));
			write_directory(to, to.blocks[0]);
			list.pop();
		}
		else
		{
			queue <pair<string, size_t> > temp;
			size_t free_block = alloc_block();
			DirectoryD new_dir(list.front().first, time(0), free_block, temp);
			vector <size_t> blocks;
			blocks.push_back(free_block);
			new_dir.blocks = blocks;
			write_directory(new_dir, free_block);
			to.files.push(make_pair(list.front().first, free_block));
			fill_free_block(to.blocks);
			write_directory(to, to.blocks[0]);
			DirectoryD new_source = read_directory(list.front().second);
			copy_directory(new_source, new_dir);
			list.pop();
		}
	}
}

void FileSystem::copy(string  source, string to)
{
	read_config();
	read_meta_data();
	pair<string, size_t> new_elem = find_directory(source, true);
	DirectoryD to_dir = read_directory(find_directory(to, true).second);
	ifstream in(utils.path_append(root_dirname, to_string(new_elem.second)));
	size_t ident;
	in.read((char *)&ident, sizeof(size_t));
	in.close();
	size_t new_block;

	if (ident == FILE)
	{
		FileD cur_file = read_file_from_blocks(new_elem.second);
		new_block = write_file_to_blocks(cur_file);
	}
	else
	{
		queue <pair<string, size_t> > temp;
		new_block = alloc_block();
		DirectoryD new_dir(new_elem.first, time(0), new_block, temp);
		vector <size_t> blocks;
		blocks.push_back(new_block);
		new_dir.blocks = blocks;
		write_directory(new_dir, new_block);
		DirectoryD cur_dir = read_directory(new_elem.second);
		copy_directory(cur_dir, new_dir);
	}

	to_dir.files.push(make_pair(new_elem.first, new_block));
	to_dir.time = time(0);
	fill_free_block(to_dir.blocks);
	write_directory(to_dir, to_dir.blocks[0]);
	write_meta();
}

size_t FileSystem::mk_one_dir(DirectoryD & parent_dir, string name)
{
	queue <pair<string, size_t> > list;
	size_t first_block_new_dir = find_first_empty();
	DirectoryD dir(name, time(0), first_block_new_dir , list);
	vector <size_t> blocks;
	blocks.push_back(first_block_new_dir);
	dir.blocks = blocks;
	write_directory(dir, first_block_new_dir);
	parent_dir.files.push(make_pair(name, first_block_new_dir));
	fill_free_block(parent_dir.blocks);
	write_directory(parent_dir, parent_dir.blocks[0]);
	write_meta();
	return first_block_new_dir;
}

void FileSystem::mkdir(string path)
{
	read_config();
	read_meta_data();
	vector <string> parts  = utils.split(path, '/');

	if(parts.size() < 1 || parts[0] != "")
		throw runtime_error("Path " + path + " should start with / - root directory");

	size_t block_num = meta_blocks_count;
	string name = "";
	size_t parts_size = parts.size() - 1;

	for(int k = 0; k != parts_size; ++k)
	{
		bool find = false;
		DirectoryD dir = read_directory(block_num);
		queue<pair<string, size_t> > temp = dir.files;
		size_t list_size = temp.size();

		for(int i = 0; i != list_size; ++i)
		{
			if(temp.front().first == parts[k + 1])
			{
				block_num = temp.front().second;
				name = temp.front().first;
				find  = true;
				break;
			}

			temp.pop();
		}

		if (!find)
		{
			block_num = mk_one_dir(dir, parts[k + 1]);
		}
	}

	write_meta();
}
void FileSystem::ls(string path)
{
	read_config();
	read_meta_data();
	pair <string, size_t> cur_ptr = find_directory(path, true);
	ifstream in(utils.path_append(root_dirname, to_string(cur_ptr.second)));
	size_t ident;
	in.read((char *)&ident, sizeof(size_t));
	in.close();

	if (ident == FILE)
	{
		FileD cur_file = read_file_from_blocks(cur_ptr.second);
		cout << "file_name: " << cur_file.name << endl << "data_size: "
		     << cur_file.data_size << endl << "last_modification: " << utils.time_to_string(cur_file.time) << endl;
	}
	else
	{
		DirectoryD cur_dir = read_directory(cur_ptr.second);
		cout << "file_name: " << cur_dir.name << endl << "last_modification: " << utils.time_to_string(cur_dir.time) << endl;
		cout << "list_files: ";
		size_t in_files_size = cur_dir.files.size();

		if (in_files_size == 0)
			cout << "no one file or subdirectory" ;
		else
		{
			queue<pair<string, size_t> > files = cur_dir.files;

			for (size_t i = 0 ; i != in_files_size; ++i)
			{
				cout << files.front().first << " ";
				files.pop();
			}
		}

		cout << endl;
	}

	write_meta();
}



size_t FileSystem::find_first_empty()
{
	size_t i = meta_blocks_count;

	if (meta_info[0] != 1)
		return -1;
	else
	{
		while(meta_info[i] == 1)
			++i;
	}

	return i;
}

size_t FileSystem::alloc_block()
{
	size_t num = find_first_empty();

	if (num != -1)
	{
		--free_blocks_count;
		meta_info[num] = 1;
		return num;
	}
	else
		return -1;
}

size_t FileSystem::alloc_block(size_t num)
{
	if (num != -1)
	{
		--free_blocks_count;
		meta_info[num] = 1;
		return num;
	}
	else
		return -1;
}



