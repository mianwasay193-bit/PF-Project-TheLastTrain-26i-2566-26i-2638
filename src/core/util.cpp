//Name: Abdul Wasay
// Roll No: i262566
//Project: The Last Train
#include "util.h"
#include <iostream>
using namespace std;

int clampInt(int val, int minVal, int maxVal)
{
    if (val < minVal)
    {
        return minVal;
    }
    else if (val > maxVal)
    {
        return maxVal;
    }
    else
    {
        return val;
    }
}  
    
bool stringEquals(const char* a, const char* b)
{
    int i;
    //Validation
    if (a == nullptr || b == nullptr) //a is a pointer to the the letters in whatever is passed, The thing passed is the address of the string.
    {
        return false; // Will return false if either string is null
    }
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return false; // Will return false if characters are not equal
        }
        i++;
    }
    return true; // Will return true if strings are equal
}

void trimWhitespace(char* str)
{
  int length,start,end,i;
  if (str == nullptr) //Validation
  {
    return; // Will return if string is null
  }
  
  //finding the length of the string
  length = 0;
  while (str[length] != '\0')
  {
    length++;
  }
  //Finding the first character that is not a space.
  start = 0;
  while (start<length && (str[start]==' ' || str[start]=='\t' || str[start]=='\n' || str[start]=='\r'))
  {
    start++;
  }
  //finding the last character that is not a space
  end = length - 1;
  while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r'))
  {
    end--;
  }
  //making the word.
  i=0;
  while (start <= end)
  {
    str[i] = str[start];
    i++;
    start++;
  }
  str[i] = '\0';
}
//Printing a header
void printHeader(const char* title)
{
    if (title == nullptr) //Validation
    {
        return; // Will return if title is null
    }
    cout << "======================" << endl;
    cout <<" " << title << endl;
    cout << "======================" << endl;
}

