#include <EEPROM.h>

#define OK              200
#define UNKNOW_ERROR    0
#define ERR_NO_SPACE    1

class File{
public:
    int index = -1;
    int e_start = -1;
    int e_end;
    int Size()
    {
        return e_end - e_start;
    }
    String Name;
    bool available()
    {
        return ((index + e_start) < e_end);
    }
    byte read()
    {
        byte b = (byte)EEPROM[e_start + index];
        index++;
        return b;
    }
    bool exists()
    {
        return e_start >= 0;
    }
};

class SmallFileSystem
{
    public:
    void Format()
    {
        for (int i = 0; i < E2END; i ++)
        {
            EEPROM.update(i, 0);
        }
    }
    void Erase(String file)
    {
        File f = Open(file);
        if (!f.exists())return;

        while (f.available())
        {
            EEPROM.update((f.index + f.e_start), 0);
            f.read();
        }
        int namel = f.Name.length();
        for (int i = 0; i < namel; i ++)
        {
            EEPROM.update(i + f.e_start - (f.Name.length() + 2), 0);
        }
    }
    int GetAvailableAddress()
    {
        bool start = false;
        for (int i = 0; i < E2END; i ++)
        {
            char ch  = (char)EEPROM[i];
            if (ch == '\0' && EEPROM[i+1] == '\0')
            {
                return i+1;
            }
        }
        return -1;
    }
    int PutFile(String file, String contents)
    {
        String str = file + "\255" + contents + "\255";
        int l = file.length() + contents.length() + 2; // /0 and /0 are 2 chars
        int idx = -1;
        File f = Open(file);
        if (f.exists())
            idx = f.e_start - (f.Name.length() + 2);
        else
            idx = GetAvailableAddress();
        if (idx == -1)return UNKNOW_ERROR;

        int fs__idx = 0;
        for (int i = 0; i < l; i ++)
        {
            int e_idx = idx+i;
            if (e_idx > E2END)
            return ERR_NO_SPACE;
            char c = str[i];
            if (c == '\255')
            EEPROM.update(e_idx, '\0');
            else
            EEPROM.update(e_idx, c);
            fs__idx = e_idx;
        }
        if (f.exists() && fs__idx < f.e_end)
        {
            int dst = (f.e_end - fs__idx);
            for (int i = fs__idx; i < f.e_end; i ++)
            {
                EEPROM.update(i, 0);
            }
        }
        return OK;

    }
    File Open(String fileName)
    {
        File f;
        bool openedName = false;
        bool openedContent = false;
        bool start = false;

        String name = "";

        int f_start;

        for (int i = 0; i < E2END; i ++)
        {
            char ch  = (char)EEPROM[i];
            if (!start)
            {
                if (ch != '\0')
                {
                    start = true;
                    openedName = true;
                    goto l_body;
                }
                continue;
            }
            l_body:
            if (ch == '\0')
            {
                if (openedName)
                {
                    // if (name == "")
                    // {
                    //     continue;
                    // }
                    openedName = false;
                    openedContent = true;
                    f_start = i+2;
                    continue;
                }
                else if (openedContent)
                {
                    if (name == fileName)
                    {
                        f.Name = name;
                        f.e_start = f_start;
                        f.e_end = i;
                        break;
                    }
                    while (EEPROM[i] == 0 && EEPROM[i+1] == 0)
                    {
                        i++;
                    }
                    openedName = true;
                    openedContent = false;
                    name = "";
                }
            }
            else if (openedName)
            {
                name += ch;
            }
            else if (openedContent)
            {
                ;
            }
        }
        return f;
    }
    //TODO: Put a FS defragger
};

SmallFileSystem SFS = SmallFileSystem();