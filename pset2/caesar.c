#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int k = 0;
char enciphered[0]; 

void getUserInput(void);
void errorMsgArgv(void);
void parseInput(string s);
char mapChar(char c, int offset);
int printEncipherText(void);

int main(int argc, string argv[])
{
  if (argc == 2)
  {
    k = atoi(argv[1]);
    if (k > 0)
    {
        getUserInput();
    }
    else
    {
        errorMsgArgv();
        return 1;
    }
  }
  else
  {
      errorMsgArgv();
      return 1;
  }
}

void errorMsgArgv(void)
{
    printf("Error: run this program by passing it a single positive integer. \n For example: ./caesar 3 \n");
}

void getUserInput(void)
{
    string s = GetString();
    if (strlen(s) > 0)
    {
      parseInput(s);
    }
    else
    {
      getUserInput();
    }
}

void parseInput(string s)
{
    for (int i = 0, len = strlen(s); i < len; i++)
    {
        if (isalpha(s[i]))
        {
            if (islower(s[i]))
            {
                enciphered[i] = mapChar(s[i], 97);
            }
            else if (isupper(s[i]))
            {
                enciphered[i] = mapChar(s[i], 65);
            }
            else
            {
                enciphered[i] = s[i];
            }
        }
        else
        {
            enciphered[i] = s[i];
        }
    }
    printEncipherText();
}
char mapChar(char c, int offset)
{
    int i = (int) c - offset;
    int shifted = (i + k) % 26;
    char cmod = (char) (shifted + offset);
    return cmod;
}
int printEncipherText()
{
    printf("%s\n", enciphered);
    return 0;
}