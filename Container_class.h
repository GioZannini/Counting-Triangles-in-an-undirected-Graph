#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

typedef unsigned int uint;

// ASSUMPTION: the first node is 0, the edges are ordered
class Container
{
public:
	// position of column for each value
	std::vector<uint> col_pos;
	// position of starting row 
	std::vector<uint> row_pos;

	Container() {}

	Container(std::string file_name, char delimiter) {
		this->initialize();
		// open file
		std::ifstream file(file_name);
		// string holds line / holds each value of line
		std::string text_line, value_line;
		// create stream to split each line by specific char
		std::stringstream stream_line("");
		// temporary container of values in each line
		uint container_line_values[2];
		// holds the position where insert the value line
		short int index_c_line;
		// control the validity of file
		if (!file.is_open())
		{
			throw std::runtime_error("Could not open file");
		}
		// starting of first row
		this->row_pos.push_back(0);
		// read file line by line
		while (std::getline(file, text_line)) {
			if (text_line[0] != '#') {
				// clear stream
				stream_line.clear();
				// add the new string line
				stream_line << text_line;
				// restart index position
				index_c_line = 0;
				// split string line associated by delimiter
				while (std::getline(stream_line, value_line, delimiter)) {
					// save value as unsigned integer
					container_line_values[index_c_line] = std::stoul(value_line);
					index_c_line++;
				}
				// insert value for column
				this->col_pos.push_back(container_line_values[1]);
				// insert right position of starting for each row
				while (this->row_pos.size() <= container_line_values[0]) {
					this->row_pos.push_back(this->col_pos.size() - 1);
				}
			}
		}
		// close file
		file.close();
	}

private:

	// initialize all vectors
	inline void initialize() {
		this->col_pos.clear();
		this->row_pos.clear();
	}


};
