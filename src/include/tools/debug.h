// The Log library for the Game Engine
#pragma once

#include <GL/glad.h>
#include <tools/utility.h>
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief The log library
 */
namespace debug
{
    // The local log-file's path
    std::string logfolder = "logs/";

    bool save_errors = true;    // save errors to the log-file ?        (recommended)
    bool save_warnings = false; // save warnings to the log-file ?      (optional)
    bool save_logs = false;     // save debugs to the log-file ?        (not recommended)
    bool save_info = false;     // save system info to the log-file ?   (optional)

    void writefile(std::string content)
    {
        // Get Date (& time)
        time_t now = time(0);
        char *dt = ctime(&now);

        std::string year = split(dt, " ", 4);
        std::string month = split(dt, " ", 1);
        std::string day = split(dt, " ", 2);

        std::cout << year << "\n"
                  << month << "\n"
                  << day << "\n";

        std::string path = logfolder + "log_" + lowercase(month) + "-" + day + ".txt";

        std::ofstream f;
        f.open(path, std::ios_base::app);
        f << content << "\n";
        f.close();
    }

    /**
     * @brief Log an error
     *
     * @param function The function
     * @param message The message
     * @param description The description
     */
    static void error(const char *function, const char *message, const char *description)
    {
        printf("error: %s; %s: %s\n", function, message, description);
        if (save_errors)
            writefile((std::string) "error: " + function + "; " + message + ": " + description);
        exit(2);
    }

    /**
     * @brief Log an error (without description)
     *
     * @param function The function
     * @param message The message
     */
    static void error(const char *function, const char *message)
    {
        printf("error: %s; %s\n", function, message);
        if (save_errors)
            writefile((std::string) "error: " + function + "; " + message);
        exit(2);
    }

    /**
     * @brief Log an error (without description & message)
     *
     * @param function The function
     */
    static void error(const char *function)
    {
        printf("error: %s\n", function);
        if (save_errors)
            writefile((std::string) "error: " + function);
        exit(2);
    }

    /**
     * @brief Log a warning
     *
     * @param function The function
     * @param message The message
     * @param description The description
     */
    static void warning(const char *function, const char *message, const char *description)
    {
        printf("warning: %s; %s: %s\n", function, message, description);
        if (save_warnings)
            writefile((std::string) "warning: " + function + "; " + message + ": " + description);
    }

    /**
     * @brief Log a warning (without description)
     *
     * @param function The function
     * @param message The message
     */
    static void warning(const char *function, const char *message)
    {
        printf("warning: %s; %s\n", function, message);
        if (save_warnings)
            writefile((std::string) "warning: " + function + "; " + message);
    }

    /**
     * @brief Log an warning (without description & message)
     *
     * @param function The function
     */
    static void warning(const char *function)
    {
        printf("warning: %s\n", function);
        if (save_warnings)
            writefile((std::string) "warning: " + function);
    }

    /**
     * @brief Log a log
     *
     * @param function The function
     * @param message The message
     * @param description The description
     */
    static void log(const char *function, const char *message, const char *description)
    {
        printf("log: %s; %s: %s\n", function, message, description);
        if (save_logs)
            writefile((std::string) "log: " + function + "; " + message + ": " + description);
    }

    /**
     * @brief Log a log (without description)
     *
     * @param function The function
     * @param message The message
     */
    static void log(const char *function, const char *message)
    {
        printf("log: %s; %s\n", function, message);
        if (save_logs)
            writefile((std::string) "log: " + function + "; " + message);
    }

    /**
     * @brief Log an log (without description & message)
     *
     * @param function The function
     */
    static void log(const char *function)
    {
        printf("log: %s\n", function);
        if (save_logs)
            writefile((std::string) "log: " + function);
    }

    /**
     * @brief Write information (the lines) in a rect
     *
     * @param lines The number of lines
     * @param line The first line
     * @param ... The rest of the lines
     */
    void rect(int lines, const char *line, ...)
    {
        va_list l;
        va_start(l, line);

        std::vector<std::string> ls;

        ls.push_back(line);

        for (int i = 0; i < lines - 1; i++)
            ls.push_back(va_arg(l, const char *));

        int width = 0;

        for (int i = 0; i < lines; i++)
            if (ls[i].size() > width)
                width = (int)ls[i].size();

        width += 4; // start and end elements

        // Draw Top
        for (int i = 0; i < width; i++)
            printf("-");
        printf("\n");

        // Draw Content
        for (int i = 0; i < lines; i++)
        {
            printf("| ");
            printf("%s", ls[i].c_str());

            int s = (int)ls[i].size();
            for (int i = 0; i < width - 4 - s; i++)
                printf(" ");

            printf(" |\n");
        }

        // Draw Bottom
        for (int i = 0; i < width; i++)
            printf("-");
        printf("\n");

        va_end(l);
    }

    /**
     * @brief Print out GL Info
     */
    static void glinfo()
    {
        std::string gl_vendor((const char *)glGetString(GL_VENDOR));
        std::string gl_renderer((const char *)glGetString(GL_RENDERER));
        std::string gl_version((const char *)glGetString(GL_VERSION));

        gl_vendor = lowercase(gl_vendor);
        gl_renderer = lowercase(gl_renderer);
        gl_version = lowercase(gl_version);

        std::string gpu = gl_renderer;
        std::string version;
        std::string profile;

        // Get Profile
        if (isin(gl_version, "(compatibility profile)"))
            profile = "compatibility";
        else if (isin(gl_version, " es "))
            profile = "es";
        else
            profile = "core";

        // Get Version
        if (profile == "es")
            version = split(gl_version, " ", 2);
        else
            version = split(gl_version, " ", 0);

        std::string _gpu = simplify(gl_renderer);
        std::string _version = "opengl " + simplify(version) + " " + profile;

        // Draw Rect
        rect(4,
             "SYSTEM INFO |",
             "-------------",
             _gpu.c_str(),
             _version.c_str());

        if (save_info)
            writefile((std::string) "system info: " + "gpu: " + _gpu + "; gl: " + _version);
    }
};