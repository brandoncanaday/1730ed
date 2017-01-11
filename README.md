# Project 2

@author - Brandon Canaday (811 737 467), Nick Angelastro (811 068 256)

## Instructions
   
   To run the program with known text filename: 

      ```
      $ make run file={FILENAME}
      ```
      Ex. 'make run file=brainstorm.txt'

      -- OR --

      ```
      $ ./p2 {FILENAME}
      ```      
      Ex. './p2 sample.txt'

   To run the program with no filename specified:
   
      ```
      $ 'make run'
      ```
   
   To compile AND link: 

      ```
      $ make
      ```
      
   To compile only: 

      ```
      $ make p2.o
      ```

   To clean dir of .o, executable, and tilda (~) files: 

      ```
      $ make clean
      ```

## Reflection

   My partner and I have come to an agreement that this was, by far, the most difficult
project that we have ever done in computer science, and that is coming from people who
have yet to make below a 100 on any project assigned to them thus far in 1301,1302, or
this class. We did not finish fixing all of the bugs associated with the editing process,
but all of the functionality of a text editor, such as save, save-as, open, quit, insert,
and backspace, as well as some added aesthetic with a teal color on some of the printed 
text. We spent well over 25 hours on this project, and, along the way, learned to utilize
low level i/o functions, such as open and read, in addition to their higher-level
counterparts, such as fopen. 