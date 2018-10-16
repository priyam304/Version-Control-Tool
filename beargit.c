#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - Here are some of the helper functions from util.h:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.

 *    full set of tests that we will run on your code. See "Step 5" in the project spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);

  FILE* fbranches = fopen(".beargit/.branches", "w");
  fprintf(fbranches, "%s\n", "master");
  fclose(fbranches);

  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");
  write_string_to_file(".beargit/.current_branch", "master");

  return 0;
}



/* beargit add <filename>
 *
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR:  File <filename> has already been added.
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR:  File %s has already been added.\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the project spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  
  FILE *file_reader=fopen(".beargit/.index","r");
  if(file_reader==NULL){
    fprintf(stderr,"File Doesn't exist");
    return 1;
  }
  else{
  char filename[512];
  int file_count=0;
  fprintf(stdout,"Tracked files:\n\n");
      while(fgets(filename,FILENAME_SIZE,file_reader)!=NULL){
        fprintf(stdout,"%s",filename);  
        file_count++;
      }
  fprintf(stdout,"\nThere are %d files total.\n",file_count);
  }
  fclose(file_reader);
  return 0;
}

/* beargit rm <filename>
 *
 * See "Step 2" in the project spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  
  FILE *findex=fopen(".beargit/.index","r");
  FILE *fnewindex=fopen(".beargit/.newindex","w");

  char line[FILENAME_SIZE];
  int file_found=0;
  while(fgets(line,sizeof(line),findex)){
      strtok(line,"\n");
      if(strcmp(line,filename)==0){
        file_found=1;
        }else{
          fprintf(fnewindex,"%s\n",line);
      
        }
      }
   fclose(findex);
   fclose(fnewindex);
   if(file_found){
      fs_mv(".beargit/.newindex",".beargit/.index");
   }else{
      fprintf(stderr,"ERROR:  File %s not tracked.\n",filename);
      fs_rm(".beargit/.newindex");
      return 3;
   }

  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the project spec.
 *
 */

const char* go_bears = "THIS IS BEAR TERRITORY!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  if(strstr(msg,go_bears)!=NULL){
    return 1;
  }
  return 0;
}

/* Use next_commit_id to fill in the rest of the commit ID.
 *
 * Hints:
 * You will need a destination string buffer to hold your next_commit_id, before you copy it back to commit_id
 * You will need to use a function we have provided for you.
 */

void next_commit_id(char *commit_id_raw) {
     /* COMPLETE THE REST */
    char commit_id[COMMIT_ID_SIZE];
    cryptohash(commit_id_raw,commit_id);    
    strcpy(commit_id_raw,commit_id);
  
  }

int beargit_commit(const char* msg) {
  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR:  Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char prev_commit_id[COMMIT_ID_SIZE];
  char branchname[BRANCHNAME_SIZE];
  char commit_id[COMMIT_ID_SIZE+BRANCHNAME_SIZE];
  char path_commit[(FILENAME_SIZE*3)+3];
  
  FILE *findex;


  read_string_from_file(".beargit/.prev", prev_commit_id, COMMIT_ID_SIZE);
  read_string_from_file(".beargit/.current_branch",branchname,BRANCHNAME_SIZE);
  strcpy(commit_id,prev_commit_id);
  strcat(commit_id,branchname);
  next_commit_id(commit_id);

  /* COMPLETE THE REST */
  strcpy(path_commit,".beargit/");
  strcat(path_commit,commit_id);
  fs_mkdir(path_commit);
  
  strcat(path_commit,"/");
  fs_cp(".beargit/.index",path_commit,".index");
  fs_cp(".beargit/.prev",path_commit,".prev");


  findex=fopen(".beargit/.index","r");
  if(findex!=NULL){
      char filename[FILENAME_SIZE];
      while(fgets(filename,sizeof(filename),findex)){
          strtok(filename,"\n");
          fs_cp(filename,path_commit,filename);
        }

  }

  fclose(findex);

  strcat(path_commit,".msg");
  findex=fopen(path_commit,"w");
  fprintf(findex, "%s\n",msg);
  fclose(findex);

  findex=fopen(".beargit/.prev","w");
  fprintf(findex,"%s\n",commit_id);
  fclose(findex);
  

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the project spec.
 *
 How  this works:
 Read commit id from .beargit/.prev file and store it in commit_id.
 Check if there is a directory corresponding to commit_path (if not print error message)
LABEL:go inside directory.
    Read from .msg file in .beargit/<commit-id>/
    print the commit and message to stdout
    Read from .prev file in .beargit/<commit-id>/
    change the commit path
    decreament the limit if provided 
    check if the directory corresponding to commit_path exist and limit >0(if not the stop)
    Repeat LABEL;
 */

int beargit_log(int limit) {
  /* COMPLETE THE REST */
  char commit_id[COMMIT_ID_SIZE];
  char commit_msg[MSG_SIZE];
  char commit_path[(COMMIT_ID_SIZE)+25]; //size= ".beargit/<Commit_ID_SIZE>/.prev"
  char msg_path[(COMMIT_ID_SIZE)+25]; //size= ".beargit/<Commit_ID_SIZE>/.msg"
  const char *beargit=".beargit/";



  strcpy(commit_path,beargit);
  read_string_from_file(".beargit/.prev",commit_id,sizeof(commit_id));
  strtok(commit_id,"\n");
  strcat(commit_path,commit_id);
  if(!fs_check_dir_exists(commit_path)){
    fprintf(stderr, "ERROR:  There are no commits.\n");
  }else{
    do{
      //clearing the msg_path string to change it
      strcpy(msg_path,commit_path);
      strcat(msg_path,"/.msg");
      read_string_from_file(msg_path,commit_msg,sizeof(commit_msg));
      strtok(commit_msg,"\n");
      fprintf(stdout, "commit %s\n    %s\n\n",commit_id,commit_msg);


      strcat(commit_path,"/.prev");
      read_string_from_file(commit_path,commit_id,sizeof(commit_id));
      strtok(commit_id,"\n");
     
     //clearing commit_path string to change it.
      strcpy(commit_path,beargit);
      strcat(commit_path,commit_id);
      limit--;
    }while(fs_check_dir_exists(commit_path)&& limit>0);
  }
  return 0;
}


// This helper function returns the branch number for a specific branch, or
// returns -1 if the branch does not exist.
int get_branch_number(const char* branch_name) {
  FILE* fbranches = fopen(".beargit/.branches", "r");

  int branch_index = -1;
  int counter = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), fbranches)) {
    strtok(line, "\n");
    if (strcmp(line, branch_name) == 0) {
      branch_index = counter;
    }
    counter++;
  }

  fclose(fbranches);

  return branch_index;
}

/* beargit branch
 *
 * See "Step 5" in the project spec.
 *
 */

int beargit_branch() {
  /* COMPLETE THE REST */

  return 0;
}

/* beargit checkout
 *
 * See "Step 6" in the project spec.
 *
 */

int checkout_commit(const char* commit_id) {
  /* COMPLETE THE REST */
  return 0;
}

int is_it_a_commit_id(const char* commit_id) {
  /* COMPLETE THE REST */
  return 1;
}

int beargit_checkout(const char* arg, int new_branch) {
  // Get the current branch
  char current_branch[BRANCHNAME_SIZE];
  read_string_from_file(".beargit/.current_branch", "current_branch", BRANCHNAME_SIZE);

  // If not detached, leave the current branch by storing the current HEAD into that branch's file...
  if (strlen(current_branch)) {
    char current_branch_file[BRANCHNAME_SIZE+50];
    sprintf(current_branch_file, ".beargit/.branch_%s", current_branch);
    fs_cp(".beargit/.prev", current_branch_file,NULL);
  }

   // Check whether the argument is a commit ID. If yes, we just change to detached mode
  // without actually having to change into any other branch.
  if (is_it_a_commit_id(arg)) {
    char commit_dir[FILENAME_SIZE] = ".beargit/";
    strcat(commit_dir, arg);
    // ...and setting the current branch to none (i.e., detached).
    write_string_to_file(".beargit/.current_branch", "");

    return checkout_commit(arg);
  }



  // Read branches file (giving us the HEAD commit id for that branch).
  int branch_exists = (get_branch_number(arg) >= 0);

  // Check for errors.
  if (!(!branch_exists || !new_branch)) {
    fprintf(stderr, "ERROR:  A branch named %s already exists.\n", arg);
    return 1;
  } else if (!branch_exists && new_branch) {
    fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
    return 1;
  }

  // Just a better name, since we now know the argument is a branch name.
  const char* branch_name = arg;

  // File for the branch we are changing into.
  char* branch_file = ".beargit/.branch_";
  strcat(branch_file, branch_name);

  // Update the branch file if new branch is created (now it can't go wrong anymore)
  if (new_branch) {
    FILE* fbranches = fopen(".beargit/.branches", "a");
    fprintf(fbranches, "%s\n", branch_name);
    fclose(fbranches);
    fs_cp(".beargit/.prev", branch_file,NULL);
  }

  write_string_to_file(".beargit/.current_branch", branch_name);

  // Read the head commit ID of this branch.
  char branch_head_commit_id[COMMIT_ID_SIZE];
  read_string_from_file(branch_file, branch_head_commit_id, COMMIT_ID_SIZE);

  // Check out the actual commit.
  return checkout_commit(branch_head_commit_id);
}

/* beargit reset
 *
 * See "Step 7" in the project spec.
 *
 */

int beargit_reset(const char* commit_id, const char* filename) {
  if (!is_it_a_commit_id(commit_id)) {
      fprintf(stderr, "ERROR:  Commit %s does not exist.\n", commit_id);
      return 1;
  }

  // Check if the file is in the commit directory
  /* COMPLETE THIS PART */

  // Copy the file to the current working directory
  /* COMPLETE THIS PART */

  // Add the file if it wasn't already there
  /* COMPLETE THIS PART */

  return 0;
}

/* beargit merge
 *
 * See "Step 8" in the project spec.
 *
 */

int beargit_merge(const char* arg) {
  // Get the commit_id or throw an error
  char commit_id[COMMIT_ID_SIZE];
  if (!is_it_a_commit_id(arg)) {
      if (get_branch_number(arg) == -1) {
            fprintf(stderr, "ERROR:  No branch or commit %s exists.\n", arg);
            return 1;
      }
      char branch_file[FILENAME_SIZE];
      snprintf(branch_file, FILENAME_SIZE, ".beargit/.branch_%s", arg);
      read_string_from_file(branch_file, commit_id, COMMIT_ID_SIZE);
  } else {
      snprintf(commit_id, COMMIT_ID_SIZE, "%s", arg);
  }

  // Iterate through each line of the commit_id index and determine how you
  // should copy the index file over
   /* COMPLETE THE REST */

  return 0;
}
