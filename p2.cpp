#include <cstdlib>
#include <ncurses.h>
#include <csignal>
#include <string>
#include <vector>
#include <fcntl.h>
#include <fstream>

using namespace std;

// __________ Prototypes __________ //

/**
 * Initializes ncurses and stdscr with the necessary flags.
 */
void init_stdscr();

/**
 * Prints the filename to stdscr (DOES NOT REFRESH ANYTHING).
 *
 * @param the window being printed to
 */
void printFileName(WINDOW *);

/**
 * Prints the prompt that tells the user they can press F1 to open the menu. (DOES NOT REFRESH ANYTHING)
 *
 * @param The window being printed to
 */

void printMenuPrompt(WINDOW *);

/**
 * Prints the title of the editor to stdscr (DOES NOT REFRESH ANYTHING).
 *
 * @param the window being printed to
 */

void printTitle(WINDOW *);

/**
 * Converts the global fileptr into a pointer to a string vector containing the lines of given file as string elements.
 *
 * @return a pointer to vector of strings containing line-by-line file content, the size of which is the num of lines in file
 */
vector<string> * fileToVector();

/**
 * Converts a string vector into lines of a file when the file is being saved. 
 *
 * @param the name of the file
 * @param the string vector to be written
 */

void vectorToFile(string, vector<string> *);

/**
 * Extracts the file content contained in the vector line-by-line, and then prints it to the screen (DOES NOT REFRESH ANYTHING).
 *
 * @param the window to print to
 * @param the reference to the vector<string> from which to extract file content
 * @param start the line to begin printing from
 * @param end the line to end printing at
 */
void printVectorFile(WINDOW *, vector<string> &, int start, int end);

/**
 * If possible, converts the global fileptr to a vector<string>, prints the vector to the given window, and returns a pointer
 * to said vector. If not possible, prints error message (DOES NOT REFRESH ANYTHING).
 *
 * @param the window to print to
 * @param the file to attempt to convert into a vector and print to the given window
 * @return a nullptr if unsuccessful. otherwise, a pointer to a vector<string> containing file contents
 */
vector<string> * openFile(WINDOW *);

/**
 * Listens for a keyboard press, and delegates behavior depending on which key was pressed.
 *
 * @param win the main file text window
 * @param menu the F1 menu inside the main text window
 * @param borderWin the window that gives a border to text window
 * @param winContent the vector<string> whose lines will be manipulated on the text window
 */
void winListen(WINDOW * win, WINDOW * menu, WINDOW * borderWin, vector<string> * winContent);

/**
 * Creates new window with some intial initial settings.
 *
 * @param the parent window from which to derive max-y/max-x
 * @return the new window
 */
WINDOW * makeWin(WINDOW *);

/**
 * Creates new window with border to contain main text window. Aesthetic purposes only.
 *
 * @param the parent window from which to derive max-y/max-x
 * @return the new border window
 */
WINDOW * makeBorderWin(WINDOW * parent);

/**
 * Creates menu inside the given parent, with user options.
 *
 * @param the parent window 
 */
WINDOW * makeMenu(WINDOW *);

/**
 * Fills the menu window with the appropriate text, which is determined by the available key options.
 * 
 * @param the menu window to fill
 */

void fillMenu(WINDOW *);

// _________ Globals ________ //

FILE * fileptr = nullptr;
string fileName;


// __________ Main __________ //

int main(int argc, char * argv[]) {
  vector<string> * fileContent = nullptr; // will contain lines of txt file, given initial size of 1 as default
  WINDOW * win; // main txt display window
  WINDOW * menu; // F(1) menu window
  WINDOW * borderWin; // the border window
  
  init_stdscr();
  
  borderWin = makeBorderWin(stdscr); // border win
  win = makeWin(stdscr); // main txt window
  menu = makeMenu(win); // F(1) menu


  // -------------------- MAIN -------------------- //


  if(argc == 2) { // CASE: SPECIFIED FILENAME
    fileName = argv[1];
    fileptr = fopen(argv[1], "rw+"); // tries to extract stream from filename
    fileContent = openFile(win); // tries to add file content and pressKeyPrompt content to window, stores either nullptr or
    printTitle(stdscr);
    printFileName(stdscr);
    printMenuPrompt(stdscr);
    refresh();
    wrefresh(borderWin);
    wrefresh(win);
    winListen(win,menu,borderWin,fileContent); // listens for char typed in win
  } else if(argc == 1) { // CASE: UNSPECIFIED FILENAME
    fileName = "untitled.txt";
    fileptr = fopen(fileName.c_str(), "w+");
    fileContent = openFile(win);
    printTitle(stdscr);
    printFileName(stdscr);
    printMenuPrompt(stdscr);
    refresh();
    wrefresh(borderWin);
    wrefresh(win);
    mvcur(0,0,0,0);
    winListen(win,menu,borderWin,fileContent);    
  } else { // CASE: TOO MANY ARGS
    printw("Correct program syntax: 'make run file={FILENAME}', or 'make run'.");
    printMenuPrompt(stdscr);
    refresh();
    winListen(win,menu,borderWin,fileContent);    
  }
  return EXIT_SUCCESS;
} // main



// __________ Definitions __________



// PRINT FUNCTIONS



void printFileName(WINDOW * stdscr) {
  int y,x;
  getmaxyx(stdscr,y,x);
  wattron(stdscr,A_BOLD | COLOR_PAIR(1));
  mvwprintw(stdscr,y-1,0,fileName.c_str());
  wattroff(stdscr,A_BOLD | COLOR_PAIR(1));
} // printFileName

void printTitle(WINDOW * stdscr) {
  int x = getmaxx(stdscr);
  string title = "CSCI_1730 Editor";
  wattron(stdscr,A_BOLD | COLOR_PAIR(1));
  mvwprintw(stdscr,0,x/2-title.size()/2,title.c_str());
  wattroff(stdscr,A_BOLD | COLOR_PAIR(1));  
} // printTitle

void printMenuPrompt(WINDOW * stdscr){
  int x = getmaxx(stdscr);
  string prompt = "<Press F1 for menu>";
  wattron(stdscr, A_BOLD | COLOR_PAIR(1));
  mvwprintw(stdscr, 0, x-1-prompt.size(), prompt.c_str());
  wattroff(stdscr,A_BOLD | COLOR_PAIR(1));  
}//printMenuPrompt

// FILE I/O



vector<string> * fileToVector() {
  vector<string> * content = new vector<string>();
  string str = "";
  char c;
  while((c = fgetc(fileptr)) != EOF) {
    if(c != '\n') {
      if(c != '\t') { str += c; } else { str += "    "; }
    } else {
      str += c;
      content->push_back(str);
      str = "";
    }//else
  }//while  
  content->push_back("");
  return content;
} // fileToVector

void vectorToFile(string str, vector<string> * content){
    if(fileptr != nullptr) { fclose(fileptr); } // makes way for new chosen file
    fileptr = fopen(str.c_str(), "rw+");
    for(vector<string>::iterator i = content->begin(); i != content->end(); i++) {
      fprintf(fileptr, (*i).c_str());
    }//for         
}//vectorToFile

void printVectorFile(WINDOW * win, vector<string> * content, int start, int end) {
  int count = 0;
  unsigned int cols = getmaxx(win)+1;
  if(!content->empty()) {
    for(vector<string>::iterator i = content->begin() + start; i != content->end(); i++) {
      if(count < end) {
    if((*i)[0] == '#') { wattron(win, COLOR_PAIR(1)); } // adds color
    if((*i).size() > cols) {
      waddstr(win, (*i).substr(0,cols).c_str());
    } else {
      waddstr(win, (*i).c_str()); // converts line in vector<string> to c_str() to be added to ncurses window    
    }
    if((*i)[0] == '#') { wattroff(win, COLOR_PAIR(1)); } // ends color
    count++;
      } else {
    break;
      }//else
    }//for
  }//if
} // printVectorFile

vector<string> * openFile(WINDOW * win) {
  vector<string> * content = nullptr;
  unsigned int y = getmaxy(win);
  if(fileptr == nullptr) {
    move(0,0);
    clrtoeol();
  } else {
    content = fileToVector();
    printVectorFile(win,content,0,y-1);
    if(content->size() > y-1) { 
      if(content->at(y-2).size() > 0) { wmove(win,y-2,content->at(y-2).size()-1); } else { wmove(win,y-2,0); }
    }//if 
  }//else
  return content;
} // openFile



// KEY LISTENER FOR AFTER FILE HAS BEEN OPENED



void winListen(WINDOW * win, WINDOW * menu, WINDOW * borderWin, vector<string> * winContent) {
  int c;
  unsigned int cLINE = 0, cCOL = 0, cLINE_old = 0, cCOL_old = 0,  maxLINE = 0, maxCOL = 0, lines = 0;
  unsigned int startLine = 0, endLine = 0, vectorLine = 0;
  bool exit = false;
  bool menuOpen = false;
  getmaxyx(win,maxLINE,maxCOL);
  lines = maxLINE+1;
  endLine = maxLINE-1;  
  noecho();
  // sets vectorLine to appropriate initial value
  if(winContent->size() > lines-1) {
    vectorLine = maxLINE-2;
  } else {
    vectorLine = winContent->size()-1;
  }//else 
  while(1) {
    cLINE_old = cLINE;
    cCOL_old = cCOL;
    getyx(win,cLINE,cCOL); // gets current curs position   
    getmaxyx(win,maxLINE,maxCOL); // gets current size of term
    lines = maxLINE+1;
    c = wgetch(win);
    switch(c) {
    case KEY_RESIZE: 
      delwin(win);
      delwin(borderWin);
      endwin(); 
      refresh();
      clear();
      printFileName(stdscr);
      printTitle(stdscr);
      printMenuPrompt(stdscr);
      refresh();
      borderWin = makeBorderWin(stdscr);
      win = makeWin(stdscr);
      wrefresh(borderWin); 
      getmaxyx(win,maxLINE,maxCOL); // UPDATE VECTORLINE W (MAXLINE - MAXLINE_OLD)
      if(winContent->size() > lines-1) {
    vectorLine = maxLINE-2;
      } else {
    vectorLine = winContent->size()-1;
      }//else 
      endLine = maxLINE-1;      
      printVectorFile(win,winContent,startLine,endLine);
      wrefresh(win); 
      break; 
    case KEY_F(1):
      if(!menuOpen) {
    menuOpen = true;
    fillMenu(menu);
    wrefresh(menu);
    wrefresh(win);
    refresh();
      } else {
    menuOpen = false; 
    wclear(menu);
    wrefresh(menu);
    wmove(win,0,0);
    if(!winContent->empty() && winContent != nullptr) { 
      printVectorFile(win,winContent,startLine,endLine); 
      wmove(win, cLINE_old, cCOL_old);
    }//if
    refresh();
    wrefresh(win);
      }
      break;
    case KEY_UP:
      if(!menuOpen && !winContent->empty() && winContent != nullptr) { // move cursor up 1
    if(cLINE > 0) {
      wmove(win,cLINE-1,winContent->at(vectorLine-1).size()-1); 
      vectorLine--;
      // makes cursor jump to end of previous line instead of empty white space
    } else if(cLINE == 0) { // scroll up 1
      if(startLine != 0) { // if not at beginning of file already
        startLine--;
        endLine--;
        vectorLine-=2; 
        printVectorFile(win,winContent,startLine,endLine);
        wmove(win,0,0);        
      }//if
    }//else if
    refresh();
    wrefresh(borderWin);
    wrefresh(win);
      }
      break;
    case KEY_DOWN:
      if(!menuOpen && !winContent->empty() && winContent != nullptr) { // move cursor down 1
    if(cLINE < maxLINE-2 && cLINE < winContent->size()-1) {
      wmove(win,cLINE+1,winContent->at(vectorLine+1).size()-1); 
      vectorLine++;
      // makes cursor jump to end of next line instead of empty white space
    } else if(cLINE == maxLINE-2) { // scroll down
      if(endLine != winContent->size()-1) { // if size of file isnt exactly endLine
        startLine++;
        endLine++;
        vectorLine+=2;
        printVectorFile(win,winContent,startLine,endLine);
        wmove(win,maxLINE-2,0);
      }//if
    }//else if
    refresh();
    wrefresh(borderWin);
    wrefresh(win);
      }
      else if(menuOpen){
    vectorToFile(fileName, winContent);
    wmove(menu, 10, 3);
    wprintw(menu, "File saved. Hit F1 to continue editing.");
    wrefresh(menu);
      }
      break;
    case KEY_RIGHT:
      if(menuOpen) { // open file
    char str[50]; // filename can't exceed 49 chars (not 50 bc of null char at end).    
    menuOpen = false; 
    wclear(menu);
    wrefresh(menu);
    wclear(win);
    wrefresh(win); // clears win data for new file content
    clear();
    refresh();
    echo(); // allows user to see chars they are typing
    
    move(0,0);
    
    bool isFirstTry = true;
    string prompt = "";    
    do {
      if(isFirstTry) { 
        prompt = "Please enter the name of the text file you would like to open: ";
        isFirstTry = false;
      } else {
        prompt = "That file does not exist. Please enter another filename: ";
      }    
      printw(prompt.c_str()); // enter filename
      getstr(str); // basically successive calls to getch() until newline char is reached (cannot exceed size of str[50])
      clear(); // clears stdscr data for new file info/prompt
      
      if(fileptr != nullptr) { fclose(fileptr); } // makes way for new chosen file
      
      fileptr = fopen(str, "r");
    } while(fileptr == nullptr);
    
    delete winContent; // makes way for new vector<string> containing new file content
    fileName = str;
    winContent = openFile(win);
    // sets vectorLine to appropriate value
    if(winContent->size() > lines-1) {
      vectorLine = maxLINE-2;
    } else {
      vectorLine = winContent->size()-1;
    }//else
    printTitle(stdscr);
    printFileName(stdscr);
    printMenuPrompt(stdscr);
    refresh();
    delwin(borderWin);
    borderWin = makeBorderWin(stdscr);
    wrefresh(borderWin);
    int winy,winx;
    getyx(win,winy,winx);
    mvcur(0,0,winy,winx);    
    
    wrefresh(win);
    noecho();
      } else { // move cursor
    if(cCOL < maxCOL-1 && cCOL < winContent->at(vectorLine).size()-1) { wmove(win,cLINE,cCOL+1); wrefresh(win); }
      }
      break;
    case KEY_LEFT:
      if(menuOpen) { // save file
    
    char str[50]; // filename can't exceed 49 chars (not 50 bc of null char at end).    
    menuOpen = false; 
    wclear(menu);
    wrefresh(menu);
    wclear(win);
    wrefresh(win); // clears win data for new file content
    clear();
    refresh();
    echo(); // allows user to see chars they are typing
    move(0,0);
    
    bool isFirstTry = true;
    string prompt = "";
    do{
      if(isFirstTry){
        prompt = "Please enter a name for the file being saved, before exiting: ";
        isFirstTry = false;
      }//if
      else {
        prompt = "Please enter a valid file name: ";
        //make error window appear
      }//else
      printw(prompt.c_str()); // enter filename
      getstr(str);
      clear();
      if((fileptr = fopen(str, "r")) == nullptr){
        if(fileptr != nullptr) { fclose(fileptr); } // makes way for new chosen file
        fileptr = fopen(str, "w+");
        vectorToFile(str, winContent);
        delwin(menu);
        delwin(win);
        delwin(borderWin);
        delete winContent;
        endwin();
        exit = true;
      }
      else {
        noecho();    
        printw("File already exists. Are you sure you want to overwrite? y/n");
        char ch;
        do{ 
          ch = getch();
          if(ch == 'y'){
        if(fileptr != nullptr) { fclose(fileptr); } // makes way for new chosen file
        fileptr = fopen(str, "w+");
        vectorToFile(str, winContent);
          }//if
        } while(ch != 'y' && ch != 'n');
        refresh();
        delwin(menu);
        delwin(win);
        delwin(borderWin);
        delete winContent;
        endwin();
        exit = true;
        
      }//else
    } while(fileptr == nullptr);
      } else { // move cursor
    if(cCOL > 0) { wmove(win,cLINE,cCOL-1); wrefresh(win); }    
      } 
      break;
    case 'q': 
      if(menuOpen){
    exit = true; 
    delwin(menu); 
    delwin(win);
    delwin(borderWin);
    delete winContent;
    endwin(); 
      }//if
    break;
    case KEY_BACKSPACE:
      if(!menuOpen) {
    if(cCOL > 0) { 
      winContent->at(vectorLine).erase(cCOL-1,1);
      cLINE_old = cLINE;
      cCOL_old = cCOL;
      wclear(win);
      printVectorFile(win, winContent, startLine, endLine);
      wmove(win, cLINE_old, cCOL_old-1);
    } else {
      if(winContent->size() > 1) { 
        winContent->erase(winContent->begin()+vectorLine); 
        cLINE_old = cLINE;
        cCOL_old = cCOL;
        wclear(win);
        printVectorFile(win, winContent, startLine, endLine);
        wmove(win, cLINE_old-1, winContent->at(vectorLine-1).size()-1);        
        vectorLine--;
      } // prevents having 0 elements in vector
    }//else        
    wrefresh(win);      
      }//if
      break;
    default: // echo chars to txt window, update winContent vector<string>
      if(!menuOpen) {
    if(cCOL < maxCOL-1){
      winContent->at(vectorLine).insert(cCOL,1,c);
      if(c == '\n') { 
        winContent->insert(winContent->begin()+vectorLine+1,""); 
        vectorLine++; 
      } // when user presses enter
      cLINE_old = cLINE;
      cCOL_old = cCOL;
      wclear(win);
      printVectorFile(win, winContent, startLine, endLine);
      if(c == '\n'){ wmove(win, cLINE_old+1, 0); } 
      else { wmove(win, cLINE_old, cCOL_old+1); }
      wrefresh(win);
    }//if    
      }//if
      break;
    }
    if(exit) { break; }
  }
  echo();
} // winListen



// INIT STUFF FOR F(1) MENU/TXT WINDOW



WINDOW * makeWin(WINDOW * parent) {
  int y,x;
  getmaxyx(parent,y,x);
  WINDOW * win = newwin(y-4, x-3, 2, 1); // creates new window of current size - the room needed on top/bottom for messages/prompts
  keypad(win, TRUE);
  scrollok(win,TRUE);
  idlok(win,TRUE);
  return win;
} // makeWin

WINDOW * makeBorderWin(WINDOW * parent) {
  int y,x;
  getmaxyx(parent,y,x);
  WINDOW * borderWin = newwin(y-2, x-1, 1, 0); 
  box(borderWin,0,0);
  return borderWin;
} // makeBorderWin

WINDOW * makeMenu(WINDOW * parent) {
  int py,px;
  getmaxyx(parent,py,px);
  WINDOW * menu = newwin(py/2,px/2,(py-py/2)/2,(px-px/2)/2);
  keypad(menu, TRUE);
  return menu;
} // makeMenu

void fillMenu(WINDOW * menu) {
  box(menu,0,0);
  wmove(menu,1,1);
  wprintw(menu,"Press the corresponding key for any of the following options:");
  wmove(menu,3,3);
  wprintw(menu,"Open (->)");
  wmove(menu,4,3);
  wprintw(menu,"Save (Down Arrow Key)");
  wmove(menu,5,3);
  wprintw(menu,"Save As (<-)");
  wmove(menu,6,3);
  wprintw(menu,"Exit (q)");
} // fillMenu

void init_stdscr() {
  initscr(); // starts ncurses mode
  if(has_colors() == FALSE) {
    endwin();
    printf("Your terminal does not support color.\n");
    exit(1);
  }
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  cbreak(); // allows each char typed to be accessible by terminal instead of waiting for newline
  keypad(stdscr,TRUE); // allows F-1 and other keys to be accessed
} // init_stdscr

