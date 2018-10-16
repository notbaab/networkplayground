#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#include "console.h"
#include "datastructures/Trie.h"
#include "linenoise.h"

// keep all the command and completions in this space
static std::unordered_map<std::string, ConsoleCallback> commands;
static std::unordered_map<std::string, ConsoleCallback> exitCommands;
static std::unordered_map<std::string, std::vector<std::string>> hints_map;
static DictionaryTrie hintTrie = DictionaryTrie();

void add_command(std::string full_command, ConsoleCallback callback)
{
    commands.emplace(full_command, callback);

    if (!hintTrie.insert(full_command, 1))
    {
        std::cout << "Failed!" << std::endl;
    }
}

void add_exit_command(std::string full_command, ConsoleCallback callback)
{
    exitCommands.emplace(full_command, callback);
}

// I want to use a ternary search tree for this
void completion(const char* buf, linenoiseCompletions* lc)
{
    std::string s(buf);
    std::vector<std::string> completions = hintTrie.predictCompletions(s, 5);

    for (std::string completion : completions)
    {
        linenoiseAddCompletion(lc, completion.c_str());
    }
}

char* hints(const char* buf, int* color, int* bold)
{
    if (!strcasecmp(buf, "hello"))
    {
        *color = 35;
        *bold = 0;
        return (char*)" World";
    }
    return NULL;
}

void interactive_console()
{
    char* line;

    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    while ((line = linenoise("cmd> ")) != NULL)
    {

        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/')
        {
            if (commands.find(line) == commands.end())
            {
                if (exitCommands.find(line) != exitCommands.end())
                {
                    // pack it up, we out
                    auto exitFunc = exitCommands[line];
                    free(line);
                    exitFunc();
                    return;
                }

                printf("Unknown command: '%s'\n", line);
                continue;
            }

            commands[line]();
            linenoiseHistoryAdd(line);
            linenoiseHistorySave("history.txt");
        }
        else if (!strncmp(line, "/historylen", 11))
        {
            int len = atoi(line + 11);
            linenoiseHistorySetMaxLen(len);
        }
        else if (line[0] == '/')
        {
            printf("Unrecognized command: %s\n", line);
        }
        free(line);
    }
}
