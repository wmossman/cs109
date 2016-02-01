// $Id: file_sys.h,v 1.4 2016-01-14 16:16:52-08 - - $

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
using namespace std;

#include "util.h"

// inode_t -
//    An inode is either a directory or a plain file.

enum class file_type {PLAIN_TYPE, DIRECTORY_TYPE};
class inode;
class base_file;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using base_file_ptr = shared_ptr<base_file>;
ostream& operator<< (ostream&, file_type);


// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      inode_ptr root {nullptr};
      inode_ptr cwd {nullptr};
	  wordvec pwd;
      string prompt_ {"% "};
   public:
		inode_ptr get_root();
		inode_ptr get_cwd();
		wordvec get_pwd();
		void set_pwd(wordvec path, bool fromRoot);
		void print_pwd(bool newLine);
		void set_prompt (const string& newPrompt);
		void mkdir(wordvec& path, bool fromRoot);
		void mkfile (wordvec& path, bool fromRoot, wordvec& words);
		void cat (wordvec& path, bool fromRoot);
		void cd (vector<string>& directory, bool fromRoot);
		void ls_cwd ();
		void ls(wordvec& path, bool fromRoot);
		inode_state();
		const string& prompt();
		inode_ptr traverse(bool fromRoot, wordvec& path);
	  
};

// class inode -
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//    

class inode {
   friend class inode_state;
   private:
      static int next_inode_nr;
      int inode_nr;
      base_file_ptr contents;
	  wordvec pwd;
   public:
		
      inode (file_type);
      int get_inode_nr() const;
	  base_file_ptr get_contents ();
	  wordvec get_pwd();
		void set_pwd(wordvec parent_pwd, string dirname);
};


// class base_file -
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.

class file_error: public runtime_error {
   public:
      explicit file_error (const string& what);
};

class base_file {
   protected:
      base_file() = default;
      base_file (const base_file&) = delete;
      base_file (base_file&&) = delete;
      base_file& operator= (const base_file&) = delete;
      base_file& operator= (base_file&&) = delete;
   public:
      virtual ~base_file() = default;
	  virtual size_t get_size() = 0;
      virtual const wordvec& readfile() const = 0;
      virtual void writefile (const wordvec& newdata) = 0;
      virtual void remove (const string& filename) = 0;
      virtual inode_ptr mkdir (const string& dirname,  inode_ptr parent) = 0;
      virtual inode_ptr mkfile (const string& filename, const wordvec words) = 0;
	  virtual void init_dirents (inode_ptr parent, inode_ptr cwd) = 0;
	  virtual void ls (wordvec& pwd) = 0;
	  virtual bool is_dir() = 0;
	  virtual void update_size() = 0;
	  virtual map<string,inode_ptr> * get_dirents() = 0;
	  virtual wordvec get_data() = 0;
};


// class plain_file -
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
// writefile -
//    Replaces the contents of a file with new contents.

class plain_file: public base_file {
   private:
      wordvec data;
	  size_t size = 0;
   public:
	  virtual size_t get_size() override;
      virtual const wordvec& readfile() const override;
      virtual void writefile (const wordvec& newdata) override;
      virtual void remove (const string& filename) override;
      virtual inode_ptr mkdir (const string& dirname, inode_ptr parent) override;
      virtual inode_ptr mkfile (const string& filename, const wordvec words) override;
	  virtual void init_dirents (inode_ptr parent, inode_ptr cwd) override;
	  virtual void ls (wordvec& pwd) override;
	  virtual bool is_dir() override{
		return false;
	  };
	  virtual void update_size();
	  virtual map<string,inode_ptr> * get_dirents() override;
	  virtual wordvec get_data() override;
};

// class directory -
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an file_error if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.

class directory: public base_file {
   private:
      // Must be a map, not unordered_map, so printing is lexicographic
      map<string,inode_ptr> dirents;
	  size_t size = 0;
   public:
	  virtual size_t get_size() override;
      virtual const wordvec& readfile() const override;
      virtual void writefile (const wordvec& newdata) override;
      virtual void remove (const string& filename) override;
      virtual inode_ptr mkdir (const string& dirname, inode_ptr parent) override;
      virtual inode_ptr mkfile (const string& filename, const wordvec words) override;
	  virtual void init_dirents (inode_ptr parent, inode_ptr cwd) override;
	  virtual void ls (wordvec& pwd) override;
	  virtual bool is_dir() override{
		return true;
	  };
	  virtual void update_size();
	  virtual map<string,inode_ptr> * get_dirents() override;
	  virtual wordvec get_data() override;
};

#endif

