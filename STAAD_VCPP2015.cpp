// STAAD_VC++.cpp : Defines the entry point for the console application.
//

// Reading joint coordinates so far, now need to read member incidences. 


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <stdio.h>
#include <typeinfo>
#include "Windows.h"

using namespace std;

int main()
{
	ifstream infile;
	char data[100];
	string output_line;
	string::size_type found, found2, found3;


	string str_space_single = "(\\s)";
	string str_space_many = "(\\s+)";
	string str_space_some = "(\\s*)";
	string str_space_opt = "(\\s)?";

	string str_digit_single = "(\\d)";
	string str_digit_many = "(\\d+)";
	string str_digit_some = "(\\d*)";
	string str_digit_opt = "(\\d)?";

	string str_dot = "(\\.)";
	string str_dot_opt = "(\\.)?";

	string str_plus_or_minus = "(\\+|-)";
	string str_plus_or_minus_opt = "(\\+|-)?";

	string str_semicolon_opt = "(;)?";
	string str_any_char_many = "(.*)";

	// A float may have a plus or minus sign, some digits, may have a dot, and then some digit
	string str_float = str_plus_or_minus_opt + str_digit_some + str_dot_opt + str_digit_some;

	// Line number is appears in the beginning and may have spaces in the beginning and a dot in the end.
	string str_line_number = str_space_some + str_digit_some + str_dot;

	// space + node number + space + x coord + space + y coord + space + z coord + ;(opt)
	string str_node_number_coords = str_space_single + str_digit_many + str_space_single + str_float + str_space_single + str_float + str_space_single + str_float + str_semicolon_opt;
	string str_node_number_coords_and = str_node_number_coords + str_any_char_many;
	string str_node_number_coords_many = "(" + str_space_single + str_digit_many + str_space_single + str_float + str_space_single + str_float + str_space_single + str_float + str_semicolon_opt + ")+";

	string str_node_data_result_line = str_line_number + str_any_char_many;

	string str_member_data_result_line = str_line_number + str_any_char_many;

	//cout<<str_node_number_coords<<"\n";
	//cout<<str_node_number_coords_many<<"\n";

	//matched array
	smatch match, next_match;

	string input = "    177. 832 0.25 1 0.75; 833 0.3 1 0.75; 834 0.25 1 0.8; 835 0.35 1 0.75";
	string str_remaining = " ";
	string node_number;
	string x_c, y_c, z_c;

	int node_num;
	float x, y, z;

	vector< vector<float> > nodes(10000, vector<float>(3));

	regex joint_line(str_node_data_result_line);
	regex member_line(str_member_data_result_line);
	regex node_coords(str_node_number_coords_and);

	infile.open("test1.anl");

	cout << "Reading from the file" << endl;
	infile >> data;

	if (infile.is_open())
	{
		while (getline(infile, output_line))
		{
			//cout << output_line << '\n';

			//cout<< "Line length is:"<<line.length();

			found = output_line.find("JOINT COORDINATES");

			if (found != string::npos)
			{
				cout << " Joint coordinates located at: " << found;
				while (true)
				{

					//Sleep(100);	
					getline(infile, output_line);
					cout << "\n" << "Next line: " << output_line;

					//Check whether the read string contains end of page 
					found2 = output_line.find("PAGE");

					// check whether the string contains member incidences --- this is to indicate the end of joint coordinates
					found3 = output_line.find("MEMBER INCIDENCES");

					if ((found2 != string::npos) || output_line.empty())
					{
						cout << output_line;
						//cout<<"\n Empty line or page end found";
						//Sleep(100);
						continue;
					}
					if (found3 != string::npos)
					{
						cout << "\n Member incidences found, read all joint coordinates";

						// Reading the member incidences
						while (true)
						{
							getline(infile, output_line);
							cout << "\n" << "Now reading after Member incidences" << output_line;

							//Check whether the read string contains end of page 
							found2 = output_line.find("PAGE");

							// check whether the string contains element incidences ---- This indicates the end of member incidences
							found3 = output_line.find("ELEMENT INCIDENCES");

							// Move on if end of page is encountered
							if ((found2 != string::npos) || output_line.empty())
							{
								cout << output_line;
								//cout<<"\n Empty line or page end found";
								//Sleep(100);
								continue;
							}

							// Break out when all member incidences are read
							if (found3 != string::npos)
							{
								cout << "\n Element Incidences found, read all member incidences";
								break;
							}

							if (output_line.find_first_not_of(' ') != string::npos)
							{

								regex_search(output_line, match, member_line);
								int match_size = match.size();

								cout << "match size: " << match_size << "\n";

								//cout <<"Line number will be stripped here. ";
								for (int i = 0; i < match.size(); i++)
								{
									cout << "match[ " << i << " ] : " << match[i] << "\n";
								}

								str_remaining = match[4];

								while (!str_remaining.empty())
								{

									regex_search(str_remaining, next_match, node_coords);

									for (int j = 0; j < next_match.size(); j++)
									{
										cout << "next_match[ " << j << " ] : " << next_match[j] << "\n";
									}

									node_number = next_match[2];
									node_num = stoi(node_number);

									x_c = next_match.str(4) + next_match.str(5) + next_match.str(6) + next_match.str(7);
									y_c = next_match.str(9) + next_match.str(10) + next_match.str(11) + next_match.str(12);
									z_c = next_match.str(14) + next_match.str(15) + next_match.str(16) + next_match.str(17);

									x = stof(x_c);
									y = stof(y_c);
									z = stof(z_c);

									cout << " Node Number: " << node_num << "\n";
									cout << " X coordinate " << x << "\n";
									cout << " Y coordinate " << y << "\n";
									cout << " Z coordinate " << z << "\n";

									nodes[node_num][0] = x;
									nodes[node_num][1] = y;
									nodes[node_num][2] = z;

									/* if(next_match[19] == " ")
									{
									cout<<"Whole line read";
									break;
									}*/

									str_remaining = next_match[19];

									//cout<<"nodes.size()"<< nodes.size() <<"\n";
								}
							}


						}
						//Sleep(100);
						break;
					}

					// Read all the joint coordinates and populate the array nodes with x, y and z coordinates.
					if (output_line.find_first_not_of(' ') != string::npos)
					{

						regex_search(output_line, match, joint_line);
						int match_size = match.size();

						cout << "match size: " << match_size << "\n";

						//cout <<"Line number will be stripped here. ";
						for (int i = 0; i < match.size(); i++)
						{
							cout << "match[ " << i << " ] : " << match[i] << "\n";
						}

						str_remaining = match[4];

						while (!str_remaining.empty())
						{

							regex_search(str_remaining, next_match, node_coords);

							for (int j = 0; j < next_match.size(); j++)
							{
								cout << "next_match[ " << j << " ] : " << next_match[j] << "\n";
							}

							node_number = next_match[2];
							node_num = stoi(node_number);

							x_c = next_match.str(4) + next_match.str(5) + next_match.str(6) + next_match.str(7);
							y_c = next_match.str(9) + next_match.str(10) + next_match.str(11) + next_match.str(12);
							z_c = next_match.str(14) + next_match.str(15) + next_match.str(16) + next_match.str(17);

							x = stof(x_c);
							y = stof(y_c);
							z = stof(z_c);

							cout << " Node Number: " << node_num << "\n";
							cout << " X coordinate " << x << "\n";
							cout << " Y coordinate " << y << "\n";
							cout << " Z coordinate " << z << "\n";

							nodes[node_num][0] = x;
							nodes[node_num][1] = y;
							nodes[node_num][2] = z;

							/* if(next_match[19] == " ")
							{
							cout<<"Whole line read";
							break;
							}*/

							str_remaining = next_match[19];

							//cout<<"nodes.size()"<< nodes.size() <<"\n";
						}
					}

					//cout<<"\n"<<"Next line: "<<output_line;
				}
			}
		}
	}
}