// $Id: file_sys.cpp,v 1.5 2016-01-14 16:16:52-08 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <string>
using namespace std;

#include "debug.h"
#include "file_sys.h"
#include "util.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}












void inode_state::set_prompt(const string& newPrompt){
	prompt_ = newPrompt;
}



inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
	inode_ptr newPointer = make_shared<inode>(inode(file_type::DIRECTORY_TYPE));
	root = newPointer;
	cwd = newPointer;
	newPointer->get_contents()->init_dirents(newPointer, newPointer);
	newPointer = NULL;
}

const string& inode_state::prompt() { return prompt_; }


void inode_state::mkdir (wordvec& path, bool fromRoot){
	string newDir = path.back();
	path.pop_back();
	inode_ptr parent = traverse(fromRoot, path);
	
	
	
	map<string, inode_ptr>::iterator mit = parent->get_contents()->get_dirents()->find(newDir);
	if(mit!=parent->get_contents()->get_dirents()->end()){
		throw file_error("file or folder already exists: " + newDir);
	}
	
	parent->get_contents()->mkdir(newDir, parent);
	
}

void inode_state::mkfile (wordvec& path, bool fromRoot, wordvec& words){
	string newFile = path.back();
	path.pop_back();
	inode_ptr parent = traverse(fromRoot, path);
	
	
	
	map<string, inode_ptr>::iterator mit = parent->get_contents()->get_dirents()->find(newFile);
	if(mit!=parent->get_contents()->get_dirents()->end()){
		if(mit->second->get_contents()->is_dir()){
			throw file_error("folder already exists: " + newFile);
		}
		else{
			mit->second->get_contents()->writefile(words);
			mit->second->get_contents()->update_size();
		}
	}
	else{
		parent->get_contents()->mkfile(newFile, words);
	}
	
	
}

void inode_state::cat (wordvec& path, bool fromRoot){
	string file = path.back();
	path.pop_back();
	inode_ptr parent = traverse(fromRoot, path);
	
	map<string, inode_ptr>::iterator mit = parent->get_contents()->get_dirents()->find(file);
	if(mit!=parent->get_contents()->get_dirents()->end()){
		if(mit->second->get_contents()->is_dir()){
			throw file_error("cannot read a directory: " + file);
		}
		else{
			for(auto it : mit->second->get_contents()->get_data()){
				cout << it << " ";
			}
			cout << endl;
		}
	}
	else{
		throw file_error("no such file: " + file);
	}
	
	
}

void inode_state::cd(wordvec& path, bool fromRoot){
	if (path.size()==0){
		cwd = root;
		set_pwd(path, true);
	}
	else{
		inode_ptr curr = traverse(fromRoot, path);
		cwd = curr;
		set_pwd(path, fromRoot);
	}
}

inode_ptr inode_state::get_root(){return root;}

inode_ptr inode_state::get_cwd(){return cwd;}


wordvec inode_state::get_pwd(){
	return pwd;
}
void inode_state::set_pwd(wordvec path, bool fromRoot){
	wordvec currPwd;
	if(!fromRoot) currPwd = get_pwd();
	for(auto it : path){
		if(it == ".");
		else if(it == ".."){
			if (currPwd.size()!=0){
				currPwd.pop_back();
			}
		}
		else{
			currPwd.push_back(it);
		}
	}
	pwd = currPwd;
	
}

void inode_state::print_pwd(bool newLine){
	print_pwd_(pwd, newLine);
}


void inode_state::ls_cwd(){
	cwd->get_contents()->ls(cwd->pwd);
}

void inode_state::ls(wordvec& path, bool fromRoot){
	inode_ptr curr = traverse(fromRoot, path);
	curr->get_contents()->ls(curr->pwd);
}

inode_ptr inode_state::traverse(bool fromRoot, wordvec& path){
	inode_ptr newPointer = fromRoot ? root : cwd;
	for(auto it : path){
		if(!newPointer->get_contents()->is_dir()){
			throw file_error("attempting to access plain file: " + it);
		}
		map<string, inode_ptr>::iterator mit = newPointer->get_contents()->get_dirents()->find(it);
		if(mit==newPointer->get_contents()->get_dirents()->end()){
			throw file_error("no such folder: " + it);
		}
		newPointer = mit->second;
	}
	return newPointer;
	//
	//how to set newPointer to NULL?
	//
}








ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}





inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

base_file_ptr inode::get_contents(){
	return contents;
}

void inode::set_pwd(wordvec parent_pwd, string dirname){
	pwd = parent_pwd;
	pwd.push_back(dirname);
}

wordvec inode::get_pwd(){
	return pwd;
}



file_error::file_error (const string& what):
            runtime_error (what) {
}



size_t plain_file::get_size(){
	return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   data = words;
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&, inode_ptr) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&, const wordvec) {
   throw file_error ("is a plain file");
}

void plain_file::init_dirents(inode_ptr, inode_ptr){
	throw file_error ("is a plain file");
}
void plain_file::ls (wordvec&){
	throw file_error ("is a plain file");
}

void plain_file::update_size(){
	size_t total = 0;
	for(wordvec::const_iterator it=data.begin();
		it!=data.end(); ++it){
		total += it->size();
	}
	
	if(data.size()>0) total += data.size() - 1;
	size = total;
}

map<string,inode_ptr> * plain_file::get_dirents() {
	return nullptr;
}

wordvec plain_file::get_data(){
	wordvec tempData = data;
	return tempData;
}






size_t directory::get_size(){
	return size;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}



inode_ptr directory::mkdir (const string& dirname, inode_ptr parent) {
    inode_ptr newPointer = make_shared<inode>(inode(file_type::DIRECTORY_TYPE));
	newPointer->get_contents()->init_dirents(parent, newPointer);
	newPointer->set_pwd(parent->get_pwd(), dirname);
	dirents.insert(pair<string, inode_ptr>(dirname, newPointer));
	newPointer = NULL;
	update_size();
    return nullptr;
}

inode_ptr directory::mkfile (const string& filename, const wordvec words) {
    inode_ptr newPointer = make_shared<inode>(inode(file_type::PLAIN_TYPE));
	newPointer->get_contents()->writefile(words);
	newPointer->get_contents()->update_size();
	dirents.insert(pair<string, inode_ptr>(filename, newPointer));
	newPointer = NULL;
	update_size();
    return nullptr;
}

void directory::init_dirents(inode_ptr parent, inode_ptr cwd){
	dirents.insert(pair<string, inode_ptr>("..", parent));
	dirents.insert(pair<string, inode_ptr>(".", cwd));
	update_size();
}

void directory::ls (wordvec& pwd){
	print_pwd_(pwd, false);
	cout<< ":" << endl;
	for (auto it : dirents){
		string name = it.first;
		if (it.second->get_contents()->is_dir() && (name!=".." && name!=".")){
			name.append("/");
		}
		
		string fsize;
		stringstream ss;
		ss << it.second->get_contents()->get_size();
		string size = ss.str();
		for(int i = 6 - size.length(); i>0; i--){
			fsize.append(" ");
		}
		fsize.append(size);
		
		string number;
		for(int i = 6 - to_string(it.second->get_inode_nr()).length(); i>0; i--){
			number.append(" ");
		}
		number.append(to_string(it.second->get_inode_nr()));
		
		cout << number << "  " << fsize << "  " << name << endl;
		
	}
}

void directory::update_size(){
	size = dirents.size();
}

map<string,inode_ptr> * directory::get_dirents() {
	map<string, inode_ptr> *mapPointer = &dirents;
	return mapPointer;
}

wordvec directory::get_data(){
	throw file_error("is directory");
}