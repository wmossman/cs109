// $Id: commands.cpp,v 1.16 2016-01-14 16:10:40-08 - - $

#include "commands.h"
#include "debug.h"
#include "util.h"

using namespace std;

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   }
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words){
	if(words.size()==1){
		throw command_error("no file specified");
	}
   for (wordvec::const_iterator it=next(words.begin());
		it!=words.end(); ++it){
		string rawPath = *it;
		bool fromRoot = rawPath[0] == '/';	
		wordvec path = split(rawPath, "/");
		state.cat(path, fromRoot);
	}
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
	if (words.size() > 2){
		throw command_error ("too many operands");
	}
	if (words.size()==1) {
		wordvec path;
		state.cd(path, true);
		return;
	}
	string rawPath = words[1];
	bool fromRoot = rawPath[0] == '/';	
	wordvec path = split(rawPath, "/");
	state.cd(path, fromRoot);
   
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
	   state.ls_cwd();
	   return;
   }
   for (wordvec::const_iterator it=next(words.begin());
		it!=words.end(); ++it){
		string rawPath = *it;
		bool fromRoot = rawPath[0] == '/';	
		wordvec path = split(rawPath, "/");
		state.ls(path, fromRoot);
	}
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    
    string rawPath = words[1];
	bool fromRoot = rawPath[0] == '/';	
	wordvec path = split(rawPath, "/");
    wordvec deck;
    copy(words.begin() + 2, words.end(), inserter(deck, deck.end()));
	state.mkfile(path, fromRoot, deck);
	
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
    for (wordvec::const_iterator it=next(words.begin());
		it!=words.end(); ++it){
		string rawPath = *it;
		bool fromRoot = rawPath[0] == '/';	
		wordvec path = split(rawPath, "/");
		state.mkdir(path, fromRoot);
	}
   
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string newPrompt;
   for (wordvec::const_iterator it=next(words.begin());
		it!=words.end(); ++it){
		newPrompt.append(*it + " ");
	}
   state.set_prompt(newPrompt);
}

void fn_pwd (inode_state& state, const wordvec& words){
	if (words.size()>1){
		cout << "ignoring operands" << endl;
	}
    state.print_pwd(true);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

