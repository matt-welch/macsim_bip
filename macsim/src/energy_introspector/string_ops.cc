#include <string>
#include <string.h>
using namespace std;

char lowercase(char ch)
{
  int alphabets = 'a'-'A';
  if((ch >= 'A') && (ch <= 'Z'))
    return ch+alphabets;
  else
    return ch;
}

char uppercase(char ch)
{
  int alphabets = 'a'-'A';
  if((ch >= 'a') && (ch <= 'z'))
    return ch-alphabets;
  else
    return ch;
}

char* lowerstring(char *str)
{
  for(int i = 0; i < strlen(str); i++)
    if((str[i] >= 'A')||(str[i] <= 'Z'))
      str[i] = lowercase(str[i]);
  return str;
}

string lowerstring(string str)
{
  return (string)lowerstring((char*)str.c_str());
}

char* upperstring(char *str)
{
  for(int i = 0; i < strlen(str); i++)
    if((str[i] >= 'a')||(str[i] <= 'z'))
      str[i] = uppercase(str[i]);
  return str;
}

string upperstring(string str)
{
  return (string)upperstring((char*)str.c_str());
}

int stricmp(const char *str1, const char *str2)
{
  unsigned char ch1, ch2;

  for (;;)
  {
    ch1 = (unsigned char)*str1++; ch1 = lowercase(ch1);
    ch2 = (unsigned char)*str2++; ch2 = lowercase(ch2);

    if (ch1 != ch2)
      return ch1 - ch2;
    if (ch1 == '\0')
      return 0;
  }
}

int stricmp(string str1, string str2)
{
  return stricmp(str1.c_str(),str2.c_str());
}
