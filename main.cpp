
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <sstream>

namespace {
    auto const IFNDEF                   = std::string("#ifndef ");
    auto const DEFINE                   = std::string("#define ");
    auto const END_HEADER               = std::string("_INCLUDED");
    auto const INCLUDE_GUARD_FOOTER     = std::string("#endif // INCLUDE_GUARD");
    
    constexpr auto const MATCH          = 0;

    struct {
        std::string const FILE              = "settings.txt";
        std::string const PROJECT_NAME_KEY  = "project-";
        std::string const INCLUDE_PATH_KEY  = "include-";
        std::string const CURRENT_PATH_KEY  = "path-";
        size_t const KEY_COUNT              = 3;

        std::string ifndefHeader;
        std::string defineHeader;

        std::string projectName;
        std::string projectIncludePath;
        std::string currentPath;
        std::string fullDirectory;
    } settings;
}

int exitFail() {
    for (auto line = std::string(); std::getline(std::cin, line);) {
        break;
    }
    return EXIT_FAILURE;
}

void displayMainMenu() {
    std::cout << std::endl << std::endl
        << "Current directory: " << settings.fullDirectory << std::endl
        << "Press L to list all headers" << std::endl
        << "Press U - update all headers" << std::endl
        << "Press M - enter menu" << std::endl
        << "Press Q - exit" << std::endl;
}

void displaySettingsMenu() {
    std::cout << std::endl << std::endl
        << "Press 1 - view settings" << std::endl
        << "Press 2 - change project name" << std::endl
        << "Press 3 - change include directory" << std::endl
        << "Press 4 - change current path" << std::endl
        << "Press X - save settings" << std::endl
        << "Press C - close menu" << std::endl;
}

void saveSettings(std::filesystem::path const & file) {
    auto ss = std::stringstream();
    std::cout << "Saving settings file..." << std::endl;

    ss << settings.PROJECT_NAME_KEY << settings.projectName << std::endl;
    ss << settings.INCLUDE_PATH_KEY << settings.projectIncludePath << std::endl;
    ss << settings.CURRENT_PATH_KEY << settings.currentPath << std::endl;

    auto out = std::ofstream(file);
    if (out.is_open()) {
        std::cout << "Writing..." << std::endl;
        out << ss.str();
        std::cout << "Finished writing" << std::endl;
        out.close();
    }
    else {
        std::cout << "Error: Output filestream is not open" << std::endl;
    }
}

bool loadSettingsFile(std::filesystem::path const & file) {
    std::cout << "Loading settings file: " << file << std::endl;
    auto line = std::string();
    auto fstream = std::ifstream(file);
    auto lineCount = size_t{ 0 };
    auto foundKeyIncludePath = false;
    auto foundKeyCurrentPath = false;
    auto foundKeyProjectName = false;

    if (fstream.is_open()) {
        std::cout << "Fstream is open" << std::endl;
        while (std::getline(fstream, line)) {
            ++lineCount;

            //std::cout << "Current line: " << line << std::endl;

            if (line.length() > settings.INCLUDE_PATH_KEY.length()) {
                if (line.substr(0, settings.INCLUDE_PATH_KEY.length()).compare(settings.INCLUDE_PATH_KEY) == MATCH) {
                    foundKeyIncludePath = true;
                    settings.projectIncludePath = line.substr(settings.PROJECT_NAME_KEY.length(), line.length() - settings.PROJECT_NAME_KEY.length());
                    std::cout << "Project include path: " << settings.projectIncludePath << std::endl;
                }
            }
            
            if (line.length() > settings.CURRENT_PATH_KEY.length()) {
                if (line.substr(0, settings.CURRENT_PATH_KEY.length()).compare(settings.CURRENT_PATH_KEY) == MATCH) {
                    foundKeyCurrentPath = true;
                    settings.currentPath = line.substr(settings.CURRENT_PATH_KEY.length(), line.length() - settings.CURRENT_PATH_KEY.length());
                    std::cout << "Current path: " << settings.currentPath << std::endl;
                    settings.fullDirectory = settings.projectIncludePath + settings.currentPath;
                    std::cout << "Full path: " << settings.fullDirectory << std::endl;
                }
            }
            
            if (line.length() > settings.PROJECT_NAME_KEY.length()) {
                if (line.substr(0, settings.PROJECT_NAME_KEY.length()).compare(settings.PROJECT_NAME_KEY) == MATCH) {
                    foundKeyProjectName = true;
                    settings.projectName = line.substr(settings.PROJECT_NAME_KEY.length(), line.length() - settings.PROJECT_NAME_KEY.length());
                    std::cout << "Project name: " << settings.projectName << std::endl;

                    settings.ifndefHeader = IFNDEF + settings.projectName + "_";
                    settings.defineHeader = DEFINE + settings.projectName + "_";
                }
            }


            if (lineCount > settings.KEY_COUNT) {
                return false;
            }
        }
        std::cout << "Finished loading settings" << std::endl;
        return true;
    }
    else {
        std::cout << "Failed" << std::endl;
        return false;
    }
}

void openMenu() {
    displaySettingsMenu();

    for (std::string line; std::getline(std::cin, line);) {
        if (line.compare("1") == MATCH) {
            std::cout << std::endl;

            std::cout << "Project name: " << settings.projectName << std::endl
                << "Include path: " << settings.projectIncludePath << std::endl
                << "Current path: " << settings.currentPath << std::endl
                << "Full directory: " << settings.fullDirectory << std::endl;
            displaySettingsMenu();
        }
        else if (line.compare("2") == MATCH) {
            std::cout << std::endl << "Enter new project name:" << std::endl;
            for (std::string line; std::getline(std::cin, line);) {
                settings.projectName = line;
                settings.ifndefHeader = IFNDEF + settings.projectName + "_";
                settings.defineHeader = DEFINE + settings.projectName + "_";
                break;
            }
            displaySettingsMenu();
        }
        else if (line.compare("3") == MATCH) {
            std::cout << std::endl << "Enter new include directory:" << std::endl;
            for (std::string line; std::getline(std::cin, line);) {
                settings.projectIncludePath = line;
                settings.fullDirectory = settings.projectIncludePath + settings.currentPath;
                break;
            }
            displaySettingsMenu();
        }
        else if (line.compare("4") == MATCH) {
            std::cout << std::endl << "Enter current path relative to include directory:" << std::endl;
            for (std::string line; std::getline(std::cin, line);) {
                settings.currentPath = line;
                settings.fullDirectory = settings.projectIncludePath + settings.currentPath;
                break;
            }
            displaySettingsMenu();
        }
        else if (line.compare("x") == MATCH) {
            std::cout << std::endl << "Saving settings..." << std::endl;
            saveSettings(settings.FILE);
            displaySettingsMenu();
        }
        else if (line.compare("c") == MATCH) {
            std::cout << std::endl << std::endl;
            break;
        }
    }
}

void convertPathToHeader(std::string & path) {
    for (auto & c : path) {
        c = std::toupper(c);
    }

    std::replace(std::begin(path), std::end(path), '/', '_');
    std::replace(std::begin(path), std::end(path), '\\', '_');
    std::replace(std::begin(path), std::end(path), '.', '_');
}

std::string getHeader(std::filesystem::path const & path, std::string const & beginning) {
    auto filename = path.string();
    auto header = filename.substr(settings.projectIncludePath.length(), filename.substr().length() - settings.projectIncludePath.length());

    convertPathToHeader(header);
    auto result = beginning + header + END_HEADER;
    return std::move(result);
}

void updateFile(std::filesystem::path const & file) {
    std::cout << "Opening file: " << file << std::endl;
    bool keepLine;
    bool hasIncludeGuardHeader = true;
    bool hasIncludeGuardFooter = false;
    bool createdNewIncludeGuard = false;
    auto ss = std::stringstream();
    auto line = std::string();
    auto fstream = std::ifstream(file);
    auto lines = size_t{ 0 };

    if (fstream.is_open()) {
        while (std::getline(fstream, line)) {
            keepLine = true;

            if (lines < 2) {
                if (hasIncludeGuardHeader) {
                    switch (lines) {
                        case 0: {
                            if (line.compare("") != MATCH) {
                                std::cout << "This file doesn't contain an include guard. Line " << lines << ": " << line << std::endl;
                                hasIncludeGuardHeader = false;
                            }
                            break;
                        }
                        case 1: {
                            if (line.substr(0, IFNDEF.length()).compare(IFNDEF) != MATCH) {
                                std::cout << "This file doesn't contain an include guard. Line " << lines << ": " << line << std::endl;
                                hasIncludeGuardHeader = false;
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }

                if (hasIncludeGuardHeader) {
                    if (line.length() > IFNDEF.length()) {

                        auto firstPart = std::string(line.substr(0, IFNDEF.length()));

                        if (firstPart == IFNDEF) {
                            keepLine = false;
                            std::cout << "Found #ifndef" << std::endl;
                            ss << getHeader(file, settings.ifndefHeader) << std::endl;
                        }
                    }

                    if (line.length() > DEFINE.length()) {
                        auto firstPart = std::string(line.substr(0, DEFINE.length()));

                        if (firstPart == DEFINE) {
                            keepLine = false;
                            std::cout << "Found #define" << std::endl;
                            ss << getHeader(file, settings.defineHeader) << std::endl;
                        }
                    }
                }
                else if (!createdNewIncludeGuard) {
                    createdNewIncludeGuard = true;

                    ss << std::endl;
                    ss << getHeader(file, settings.ifndefHeader) << std::endl;
                    ss << getHeader(file, settings.defineHeader) << std::endl;
                    ss << "#pragma once" << std::endl << std::endl;
                }

                ++lines;
            }

            if (line.length() == INCLUDE_GUARD_FOOTER.length()) {   // include guard footer
                hasIncludeGuardFooter = true;
            }

            if (keepLine) {
                ss << line << std::endl;

            }
        }

        if (!hasIncludeGuardFooter) {
            std::cout << "Include guard footer not found -- adding footer" << std::endl;
            hasIncludeGuardFooter = true;
            ss << INCLUDE_GUARD_FOOTER << std::endl;
        }

        fstream.close();
    }

    auto out = std::ofstream(file);
    if (out.is_open()) {
        std::cout << "Writing file: " << file << std::endl;
        out << ss.str();

        out.close();
    }

    std::cout << "Done" << std::endl << std::endl;
}

std::vector<std::filesystem::path> getFiles(std::filesystem::directory_entry dir) {
    std::vector<std::filesystem::path> files;
    if (dir.exists()) {
        for (auto const & entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_directory()) {
                auto nextFiles = getFiles(entry);
                files.insert(std::end(files), std::begin(nextFiles), std::end(nextFiles));
            }
            else {
                //std::cout << entry.path() << std::endl;
                files.push_back(std::move(entry));
            }
        }
    }
    return std::move(files);
}

void outputFiles(std::vector<std::filesystem::path> const & files) {
    for (auto & file : files) {
        auto filename = file.string();
        auto headerText = filename.substr(settings.projectIncludePath.length(), filename.substr().length() - settings.projectIncludePath.length());

        std::cout << "File: " << headerText << std::endl;
        
        //convertPathToHeader(headerText);
        //std::cout << "File: " << headerText << std::endl;
    }
}

void updateFiles(std::vector<std::filesystem::path> const & files) {
    for (auto & file : files) {
        std::cout << "Updating file " << file << std::endl;
        updateFile(file);
    }
}

int main()
{
    auto success = loadSettingsFile(settings.FILE);
    if (!success) {
        std::cout << "Error loading settings file -- rebuilding settings..." << std::endl;
        saveSettings(settings.FILE);
    }

    auto dirEntry = std::filesystem::directory_entry(settings.fullDirectory);

    if (!dirEntry.exists()) {
        std::cout << "Error: directory " << settings.fullDirectory << " doesn't exist" << std::endl;
        
        openMenu();
    }

    displayMainMenu();

    for (std::string line; std::getline(std::cin, line);) {
        if (line == "l") {
            outputFiles(getFiles(dirEntry));
            displayMainMenu();
        }
        else if (line == "u") {
            updateFiles(getFiles(dirEntry));
            displayMainMenu();
        }
        else if (line == "m") {
            openMenu();
            displayMainMenu();
        }
        else if (line == "q") {
            break;
        }
    }

    return EXIT_SUCCESS;
}
