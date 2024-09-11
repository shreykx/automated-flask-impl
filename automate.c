#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>


int main() {
    FILE *file = fopen("app.py", "w");
    if (file == NULL) {
        printf("Error creating file!\n");
        return 1;
    }
    
    char buffer[MAX_PATH];
    char *dir_name = NULL;  // Declare dir_name at the beginning of the function
    if (GetCurrentDirectoryA(MAX_PATH, buffer)) {        
        char *token = strtok(buffer, "\\");
        while (token != NULL) {
            dir_name = token;
            token = strtok(NULL, "\\");
        }
        
        if (dir_name != NULL) {
            printf("Intializing project: %s\n", dir_name);
            fprintf(file, "# Project: %s\n\n", dir_name);
            fprintf(file, "print('Welcome to %s')\n", dir_name);
        } else {
            printf("Error extracting directory name.\n");
            fprintf(file, "print('Error extracting directory name')\n\n");
        }
    } else {
        printf("Error getting current directory.\n");
        fprintf(file, "print('Error getting current directory')\n\n");
    }
    // Create a new Python environment in the 'env' directory
    char command[512];
    snprintf(command, sizeof(command), "python -m venv env");
    
    int result = system(command);
    if (result == 0) {
        printf("Python environment created successfully in 'env' directory.\n");
        fprintf(file, "# Python environment created in 'env' directory\n");
        
        // Activate the virtual environment and install packages
        #ifdef _WIN32
            snprintf(command, sizeof(command), "env\\Scripts\\activate && pip install flask flask_cors pymongo");
        #else
            snprintf(command, sizeof(command), "source env/bin/activate && pip install flask flask_cors pymongo");
        #endif
        
        result = system(command);
        if (result == 0) {
            printf("Packages installed successfully.\n");
            fprintf(file, "# Packages installed: flask, flask_cors, pymongo\n");
        } else {
            printf("Error installing packages.\n");
            fprintf(file, "# Error installing packages\n");
        }
    } else {
        printf("Error creating Python environment.\n");
        fprintf(file, "# Error creating Python environment\n");
    }

    // Create a string containing the code to be added to app.py
    // Create a string to store the port number
    char port[10];
    printf("PORT?\n");
    fgets(port, sizeof(port), stdin);
    
    // Remove newline character if present
    char *newline = strchr(port, '\n');
    if (newline) *newline = '\0';
    
    // Update the app_py_code to include the port and serve index.html
    const char *app_py_code = 
        "from flask import Flask, request, jsonify, render_template, send_from_directory\n"
        "from flask_cors import CORS\n"
        "app = Flask(__name__)\n"
        "CORS(app)\n\n"
        "@app.route('/')\n"
        "def index():\n"
        "    return render_template('index.html')\n\n"
        "if __name__ == '__main__':\n"
        "    app.run(host='0.0.0.0', debug=True, port=%s)\n";

    // Append the new code to app.py
    fprintf(file, "\n");
    fprintf(file, app_py_code, port);
    printf("Your app will now run on port %s\n", port);

    // Create db.py file
    FILE *db_file = fopen("db.py", "w");
    if (db_file == NULL) {
        printf("Error creating db.py file.\n");
        return 1;
    }

    // Get database name from user input
    char db_name[100];
    printf("Enter the database name: ");
    fgets(db_name, sizeof(db_name), stdin);
    
    // Remove newline character if present
    newline = strchr(db_name, '\n');
    if (newline) *newline = '\0';

    // Get MongoDB connection URL from user input
    char mongo_uri[200];
    printf("Enter the MongoDB connection URL: ");
    fgets(mongo_uri, sizeof(mongo_uri), stdin);
    
    // Remove newline character if present
    newline = strchr(mongo_uri, '\n');
    if (newline) *newline = '\0';

    const char *db_py_code = 
        "from pymongo import MongoClient\n\n"
        "# MongoDB connection string\n"
        "MONGO_URI = '%s'\n\n"
        "# Database name\n"
        "DB_NAME = '%s'\n\n"
        "def get_db():\n"
        "    client = MongoClient(MONGO_URI)\n"
        "    return client[DB_NAME]\n\n"
        "def close_db(client):\n"
        "    client.close()\n";

    fprintf(db_file, db_py_code, mongo_uri, db_name);
    fclose(db_file);

    printf("db.py file created successfully with database name: %s and MongoDB URI: %s\n", db_name, mongo_uri);

    // Check if templates directory exists
    DWORD dwAttrib = GetFileAttributesA("templates");
    if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
        printf("Templates directory already exists.\n");
    } else {
        // Create templates directory
        #ifdef _WIN32
            result = system("mkdir templates");
        #else
            result = system("mkdir -p templates");
        #endif

        if (result == 0) {
            printf("Templates directory created successfully.\n");
        } else {
            printf("Error creating templates directory.\n");
            return 1;
        }
    }

    // Create index.html file in templates directory
    FILE *index_file = fopen("templates/index.html", "w");
    if (index_file == NULL) {
        printf("Error creating index.html file.\n");
        return 1;
    }

    fprintf(index_file, "<html>\n<head>\n    <title>Welcome</title>\n</head>\n<body>\n    <h1>Welcome to your Flask app!</h1>\n</body>\n</html>");
    fclose(index_file);

    printf("index.html file created successfully in templates directory.\n");

    // Initialize git repository
    result = system("git init");
    if (result == 0) {
        printf("Git repository initialized successfully.\n");
        
        // Create .gitignore file
        FILE *gitignore_file = fopen(".gitignore", "w");
        if (gitignore_file != NULL) {
            fprintf(gitignore_file, "env/\n*.pyc\n__pycache__/\n");
            fclose(gitignore_file);
            printf(".gitignore file created successfully.\n");
        } else {
            printf("Error creating .gitignore file.\n");
        }

        // Create README.md file
        FILE *readme_file = fopen("README.md", "w");
        if (readme_file != NULL) {
            if (dir_name != NULL) {
                fprintf(readme_file, "# %s\n", dir_name);
                printf("README.md file created successfully with project name as heading.\n");
            } else {
                fprintf(readme_file, "# Project\n");
                printf("README.md file created successfully with default heading.\n");
            }
            fclose(readme_file);
        } else {
            printf("Error creating README.md file.\n");
        }
    } else {
        printf("Error initializing Git repository.\n");
    }
    // Add all files to git
    result = system("git add .");
    if (result == 0) {
        printf("All files added to git successfully.\n");
        
        // Commit the first commit
        result = system("git commit -m \"init\"");
        if (result == 0) {
            printf("Initial commit created successfully.\n");
            
            // Get remote origin from user
            char remote_origin[256];
            printf("Enter the remote origin URL: ");
            fgets(remote_origin, sizeof(remote_origin), stdin);
            
            // Remove newline character if present
            char *newline = strchr(remote_origin, '\n');
            if (newline) *newline = '\0';
            
            // Add remote origin
            char git_remote_command[512];
            snprintf(git_remote_command, sizeof(git_remote_command), "git remote add origin %s", remote_origin);
            result = system(git_remote_command);
            
            if (result == 0) {
                printf("Remote origin added successfully.\n");
                
                // Push to main branch
                result = system("git push -u origin main");
                if (result == 0) {
                    printf("Code pushed to main branch successfully.\n");
                } else {
                    printf("Error pushing code to main branch.\n");
                }
            } else {
                printf("Error adding remote origin.\n");
            }
        } else {
            printf("Error creating initial commit.\n");
        }
    } else {
        printf("Error adding files to git.\n");
    }
    return 0;
}
