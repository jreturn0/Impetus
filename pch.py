
import os
def insert_pch_header(directory):
    # Define the PCH include line
    pch_line = '#include "pch.h"\n'

    # Walk through the directory to find all .cpp files
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                
                # Read the original content
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.readlines()

                # Check if the PCH line is already present
                if content and pch_line.strip() in content[0].strip():
                    continue  # Skip files that already include pch.h

                # Remove any existing BOM (Byte Order Mark)
                if content[0].startswith('\ufeff'):
                    content[0] = content[0][1:]
                
                # Insert the PCH line at the beginning of the file
                content.insert(0, pch_line)
                
                # Write the modified content back to the file
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.writelines(content)

    print("PCH header inserted into all .cpp files in the directory.")

def remove_duplicate_pch_header(directory):
    # Define the PCH include line
    pch_line = '#include "pch.h"\n'

    # Walk through the directory to find all .cpp files
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                
                # Read the original content
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.readlines()

                # Check if the first two lines are the PCH include
                if len(content) > 1 and content[0] == pch_line and content[1] == pch_line:
                    # Remove the duplicate PCH line
                    content = content[1:]

                # Write the cleaned content back to the file
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.writelines(content)

    print("Duplicate PCH headers removed from all .cpp files in the directory.")

def remove_first_line_if_pch(directory):
    pch_line = '#include "pch.h"\n'
    
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                
                with open(file_path, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
                
                if lines and lines[0] == pch_line:
                    # Remove the first line
                    lines = lines[1:]
                
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.writelines(lines)

    print("Removed first line if it was '#include \"pch.h\"' from all .cpp files.")
def add_pch_header_cleanly(directory):
    pch_line = '#include "pch.h"\n'
    
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp'):
                file_path = os.path.join(root, file)
                
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()

                # Check if the PCH line is already present at the start
                if content.startswith(pch_line):
                    continue  # Skip files that already include pch.h

                # Insert the PCH line at the start of the file
                content = pch_line + content
                
                # Write the content back to the file without introducing any unwanted characters
                with open(file_path, 'w', encoding='utf-8', newline='') as f:
                    f.write(content)

    print("PCH header added to all .cpp files in the directory.")
if __name__ == "__main__":
    # Replace 'your_directory' with the path to the folder containing your .cpp files
    directory = 'C:\\Users\\Joshua Ross\\source\\repos\\Impetus\\Renderer\\src'
    add_pch_header_cleanly(directory)
