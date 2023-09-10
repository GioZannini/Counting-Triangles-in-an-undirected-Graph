#include "Container_class.h"
#include <algorithm>
#include <chrono>


int main(int argc, char* argv[]) {
	// contains how many thread used in the computation
	std::vector<std::pair<bool, uint>> parallel = {std::pair<bool, uint>(false, 1), std::pair<bool, uint>(true, 2), std::pair<bool, uint>(true, 4), std::pair<bool, uint>(true, 8), std::pair<bool, uint>(true, 16),
												   std::pair<bool, uint>(true, 32), std::pair<bool, uint>(true, 64), std::pair<bool, uint>(true, 128), std::pair<bool, uint>(true, 256), std::pair<bool, uint>(true, 512), 
												   std::pair<bool, uint>(true, 1024), std::pair<bool, uint>(true, 2048)};
	// contains datasets
	std::vector<std::pair<std::string, char>> data_sets = { std::pair<std::string, char>("com-amazon.ungraph.txt", '\t'),
															std::pair<std::string, char>("as-skitter.txt", '\t'),
															std::pair<std::string, char>("com-dblp.ungraph.txt", '\t'),
															std::pair<std::string, char>("com-youtube.ungraph.txt", '\t'),
															std::pair<std::string, char>("facebook_combined.txt", ' '),
															std::pair<std::string, char>("com-orkut.ungraph.txt", '\t') };
	// folder where is data
	std::string folder = "./data/";
	// create report
	std::ofstream report("report.csv");
	// headers of table
	report << "threads, name dataset, non parallel time, parallel time, total time" << "\n";

	// determine number of threads
	for (std::pair<bool, uint> pll : parallel) {
		// determine dataset
		for (std::pair<std::string, char> data : data_sets) {
			// repeat 10 times the same configuration
			for (int repeat = 1; repeat <= 5; repeat++) {
				// print info
				std::cout << "N_THREADS: " << pll.second << "\n";
				std::cout << "DATA SET: " << data.first << "\n";
				std::cout << "REPETITION N°: " << repeat << "\n";
				// start time
				auto start = std::chrono::high_resolution_clock::now();

				// read data
				Container c(folder + data.first, data.second);
				// guard of inner for loop
				uint second_guard;
				// return the number of triangles
				uint count_triangles = 0;

				/*// used in set_intersection function
				// starting of row of first node
				uint start_first_node;
				// ending of row of first node
				uint end_first_node;
				// starting of row of second node 
				uint start_second_node;
				// ending of row of second node
				uint end_second_node;*/
				
				
				// used in set_intersection function
				// starting of row of first node
				std::vector<uint>::iterator start_first_node;
				// ending of row of first node
				std::vector<uint>::iterator end_first_node;
				// starting of row of second node 
				std::vector<uint>::iterator start_second_node;
				// ending of row of second node
				std::vector<uint>::iterator end_second_node;
				// container to hold the node intersection
				std::vector<uint> v_intersection;
				
				
				// first elapsed time
				auto check_p_1 = std::chrono::high_resolution_clock::now();
				// change row, NB: -1 being that the last row can't hold triangles
				#pragma omp parallel if(pll.first) num_threads(pll.second) reduction(+:count_triangles) firstprivate(v_intersection) private(start_first_node) private(start_second_node) private(end_first_node) private(end_second_node) private(second_guard)
				for (uint pos = 0; pos < c.row_pos.size() - 1; pos++) {

					// condition of exiting from second for,
					// I don't need to watch the last connection of each node, since that
					// it can't create a triangle (because the first set is empty)
					// NB: I do in this way since that I use unsigned int so 0 - 1 return the biggest value! 
					//       					 means that  row is empty   | means row not empty and I watch all elements without last
					second_guard = c.row_pos[pos + 1] == c.row_pos[pos] ? 0 : c.row_pos[pos + 1] - 1;
					#pragma omp for schedule(dynamic) nowait
					for (uint pos_c = c.row_pos[pos]; pos_c < second_guard; pos_c++) {
						// condition that identify the case when the exist a node greater than the max node in left column
						// (is a node that has only in links)
						if (!(c.col_pos[pos_c] >= c.row_pos.size())){
							/*
							// starting of row of first node for intersection
							// I do +1 being that all nodes in intersection will be consider as triangles
							start_first_node = pos_c + 1;
							// ending of row of first node
							end_first_node = c.row_pos[pos + 1];
							// starting of row of second node 
							start_second_node = c.row_pos[c.col_pos[pos_c]];
							// means that this node doesn't have outlinks
							// ending of row of second node
							//				  means that the second node is the last row in the matrix
							end_second_node = c.col_pos[pos_c] + 1 >= c.row_pos.size() ? c.col_pos.size() : c.row_pos[c.col_pos[pos_c] + 1];
							*/
							
							// starting of row of first node
							// I do +1 being that all nodes in intersection will be consider as triangles
							// and if I don't use I will ahve a mismatch in triangles number
							start_first_node = c.col_pos.begin() + pos_c + 1;
							// ending of row of first node
							end_first_node = c.col_pos.begin() + c.row_pos[pos + 1];
							// starting of row of second node 
							start_second_node = c.col_pos.begin() + c.row_pos[c.col_pos[pos_c]];
							// ending of row of second node
							// this condition is in case the node is the last in the list 
							end_second_node = c.col_pos[pos_c] + 1 >= c.row_pos.size() ? c.col_pos.end() : c.col_pos.begin() + c.row_pos[c.col_pos[pos_c] + 1];							
							
							// determine intersection
							std::set_intersection(start_first_node, end_first_node,
								start_second_node, end_second_node,
								std::back_inserter(v_intersection));
							// increment number of triangles
							count_triangles += v_intersection.size();
							// clear vector for the next loop
							v_intersection.clear();
						}
						
					}
					if (pos % 100 == 0) {
						#pragma omp barrier
					}
				}

				auto check_p_2 = std::chrono::high_resolution_clock::now();
				// return the number of triangles inside graph
				std::cout << "TRIANGLES: " << count_triangles << "\n";

				report << pll.second << ", " << data.first << ", " << (std::chrono::duration_cast<std::chrono::milliseconds>(check_p_1 - start)).count() <<
						  ", " << (std::chrono::duration_cast<std::chrono::milliseconds>(check_p_2 - check_p_1)).count() << ", " <<
						  (std::chrono::duration_cast<std::chrono::milliseconds>(check_p_2 - start)).count() << "\n";
			}
		}
	}
	report.close();

	return 0;
}