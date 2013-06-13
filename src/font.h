/* This software is the propery of 'SaraCraft' - developed by Allanis.
 * Only members directly assosiated with the SaraCraft project may view/change
 * this code.
 */

#ifndef _FONT_H_ 
#define _FONT_H_

// Initialze the font system
// This only needs to be called once per execution of the program
bool BuildFont(char const *file);

// Initialze the font system
// This only needs to be called once per execution of the program
void DestroyFont();

void EnterTextMode(int width, int height);

void ExitTextMode();

void glPrint(char const *s, int set);

void glPrintf(int set, const char *s, ...);

void glNewLine();

#endif
