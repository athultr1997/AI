 /**
 * @file problem_solving_by_hill_climbing_search.c
 * @brief Code for solving the 'Coal Allocation Problem' described in part-1 of l2.pdf
 *        using 'Hill Climbing Search with Random Restarts'.
 * 
 * @author Athul Thaliyachira Reji
 * @date 16 May 2019
 */

#include <stdio.h> /**< For printf() */
#include <stdlib.h> /**< For fclose() and scanf() */

#define true 1 /**< For use as boolean true */
#define false 0 /**< For use as boolean false */
#define no_bid -1 /**< Used in State::selected_bids_from_companies at indices where no bid
					   is selected from the company corresponding to that index */

/**
* @brief Represents the state of the problem at any moment during the search.
* 
* The state at any moment is represented by:\n
* 1) State::selected_bids_from_companies\n
* 2) State::allocated_coal_blocks
*/
struct State
{
	int num_of_companies; /**< For storing the length of State::selected_bids_from_companies. 
							   Does not change during the search. Defined for convenience.*/
	int num_of_coal_blocks; /**< For storing the length of State::allocated_coal_blocks. 
								 Does not change during the search. Defined for convenience.*/
	int *selected_bids_from_companies; /**< Stores the indices of the selected bids from InputData. 
											It stores one bid from the set of bids proposed by 
											a company. The index value corresponding to the bid data in 
											InputData::bid_data is stored. A company from which no bid 
											is sleceted has no_bid value set at its index. Such a 
											definition was used for the ease of change inside the code 
											of generate_successor.
										 */
	int *allocated_coal_blocks; /**< Represents the allocation status of the blocks of coal: 
									 1) true: allocated 2) false: not allocated 
								  */
};

/**
* @brief Data structure to store the input data for the coal allocation problem
* 
* It stores the data as a 3D array.
*/
struct InputData
{
	int ***bid_data; /**< 3D array to store the input data. The first dimension corresponds to the 
					     company id (cid). The second dimension corresponds to the bids of the 
					     particular company set by the first dimension. The first instance in the 
					     third dimension is the bid value and the rest corresponds to the block 
					     ids of the coal. Example usages:\n
					     bid_data[i]: all the bid data corresponding to the company defined by id i+1 \n
					     bid_data[i][j]: details of (i+1)-th company's j-th bid \n
					     bid_data[i][j][0]: bid value \n
					     bid_data[i][j][k]: block id of the coal (where k>0) 
					  */
	int **bid_data_dimensions; /**< 2D array to keep track of the size of InputData::bid_data. 
									It is used since the length of each array in InputData::bid_data varies 
									and there are no ways of determining it during runtime using sizeof-like 
									functions. Hence, it has to be recorded.
								 */
	int *bid_data_dimensions_dimensions; /**< 1D array to keep track of the size of InputData::bid_data_dimensions. 
											  It is used since the length of each array in InputData::bid_data_dimensions varies 
											  and there are no ways of determining it during runtime using sizeof-like 
											  functions. Hence, it has to be recorded.
										   */ 
	int num_of_coal_blocks; /**< The number of coal blocks for auction. */
	int num_of_companies; /**< The number of companies who are participating in the auction. */
};

/**
* @brief Utility function to create a copy of a State object.
* 
* There are various places in the program where a State type variable had 
* to be copied. This function is useful in such situations.
* @param state A pointer to an instance of State whose copy is to be made
* @return new_state A pointer to a copy of state
*/
struct State *create_copy_state(struct State *state)
{
	struct State *new_state = malloc(sizeof(struct State));
	new_state->num_of_companies = state->num_of_companies;
	new_state->num_of_coal_blocks = state->num_of_coal_blocks;
	new_state->allocated_coal_blocks = malloc(new_state->num_of_coal_blocks*sizeof(int));
	new_state->selected_bids_from_companies = malloc(new_state->num_of_companies*sizeof(int));

	for(int i=0;i<new_state->num_of_coal_blocks;i++)
	{
		new_state->allocated_coal_blocks[i] = state->allocated_coal_blocks[i];
	}

	for(int i=0;i<new_state->num_of_companies;i++)
	{
		new_state->selected_bids_from_companies[i] = state->selected_bids_from_companies[i];
	}

	return new_state;
}

/**
* @brief Deallocates the memory used by an instance of struct State
* @param state The instance of struct State whose memory is to be deallocated
* @return void
*/
void deallocate_state(struct State *state)
{
	if(state->selected_bids_from_companies!=NULL)
	{
		free(state->selected_bids_from_companies);
		state->selected_bids_from_companies = NULL;		
	}
	if(state->allocated_coal_blocks!=NULL)
	{
		free(state->allocated_coal_blocks);
		state->allocated_coal_blocks = NULL;
	}
}

void display_state(struct State *state, struct InputData *input_data)
{
	printf("********STATE********\n");
	printf("num_of_companies = %d\n",state->num_of_companies);
	
	printf("selected_bids_from_companies =\n");
	for(int i=0;i<state->num_of_companies;i++)
	{
		printf("	company %d: ",i+1);
		if(state->selected_bids_from_companies[i]==no_bid)
		{
			printf("no bids selected\n");
		}
		else
		{
			for(int j=0;j<input_data->bid_data_dimensions[i][state->selected_bids_from_companies[i]];j++)
			{
				printf("%d ",input_data->bid_data[i][state->selected_bids_from_companies[i]][j]);
			}
			printf("\n");
		}
	}

	printf("allocated_coal_blocks =");
	for(int i=0;i<state->num_of_coal_blocks;i++)
	{
		printf("%d:%d,",i+1,state->allocated_coal_blocks[i]);
	}
	printf("\n");
	printf("*********************\n");
}

int cost_heuristic(struct State *state,struct InputData *input_data)
{
	int cost = 0;

	for(int i=0;i<state->num_of_companies;i++)
	{
		if(state->selected_bids_from_companies[i]!=no_bid)
		{
			cost += input_data->bid_data[i][state->selected_bids_from_companies[i]][0];
		}
	}

	return cost;
}

int check_bid_collision(int *allocated_coal_blocks,int *bid,int bid_length)
{
	for(int i=0;i<bid_length;i++)
	{
		if(allocated_coal_blocks[bid[i+1]-1]==true)
		{
			return false;
		}
	}

	return true;
}

void swap(int *x, int *y)  
{  
    int temp;  
    temp = *x;  
    *x = *y;  
    *y = temp;  
}

void permute(int *a, int l, int r)  
{  
    int i;  
    if (l == r)  
        printf("%d\n",*a);
    else
    {  
        for (i = l; i <= r; i++)  
        {  
            swap((a+l), (a+i));  
            permute(a, l+1, r);  
            swap((a+l), (a+i)); //backtrack  
        }  
    }  
}  

/**
 * @brief Utility function to shuffle the order of companies
 *
 * The companies are taken in the default order specified in the text file initially.
 * The order of the companies matters a lot because if a bid is selected 
 * from a company first it seals the coal blocks in its bid so that no 
 * other company can bid for it further. Thus, there arises a need to 
 * shuffle the order of the companies.
 *
 * @param input_data The input data taken from the text file
 * @return void
 */
void shuffle(struct InputData *input_data)
{
	int array[] = {1,2,3,4};
	permute(array,0,3);


}

struct State *generate_successor(struct State *old_state,struct InputData *input_data)
{	
	struct State *successor_state = create_copy_state(old_state);
	

	int h_max = 0;
	int h;
	
	for(int i=0;i<input_data->num_of_companies;i++)
	{
		for(int j=-1;j<input_data->bid_data_dimensions_dimensions[i];j++) /*-1 corresponds to no_bid */
		{
			if(j!=old_state->selected_bids_from_companies[i])
			{
				struct State *new_state = create_copy_state(old_state);

				int flag = true; /**< The flag is set when a new valid bid is detected from a company while 
						  			  the bids from the remaining companies stays the same */
				
				/* setting all the blocks corresponding to the currently selected bid from company i to false */
				for(int m=1;m<input_data->bid_data_dimensions[i][new_state->selected_bids_from_companies[i]];m++)
				{
					new_state->allocated_coal_blocks[input_data->bid_data[i][new_state->selected_bids_from_companies[i]][m]-1] = false;
				}

				new_state->selected_bids_from_companies[i] = j;

				if(new_state->selected_bids_from_companies[i]!=no_bid)
				{
					for(int m=0;m<input_data->bid_data_dimensions[i][new_state->selected_bids_from_companies[i]];m++)
					{
						if(new_state->allocated_coal_blocks[input_data->bid_data[i][new_state->selected_bids_from_companies[i]][m]-1] == true)
						{
							flag = false;
							break;
						}
					}
				}

				if(flag==true)
				{
					if(new_state->selected_bids_from_companies[i]!=no_bid)
					{
						for(int m=1;m<input_data->bid_data_dimensions[i][new_state->selected_bids_from_companies[i]];m++)
						{
							new_state->allocated_coal_blocks[input_data->bid_data[i][new_state->selected_bids_from_companies[i]][m]-1] = true;
						}
					}

					h = cost_heuristic(new_state,input_data);
					if(h>h_max)
					{
						h_max = h;
						deallocate_state(successor_state);
						successor_state = create_copy_state(new_state);
					}

					// for(int m=0;m<input_data->bid_data_dimensions[i][new_state->selected_bids_from_companies[i]];m++)
					// {
					// 	new_state->allocated_coal_blocks[input_data->bid_data[i][new_state->selected_bids_from_companies[i]][m+1]-1] = false;
					// 	new_state->selected_bids_from_companies[i] = old_state->selected_bids_from_companies[i];
					// }					
				}
				deallocate_state(new_state);									
			}						
		}
	}

	deallocate_state(old_state);

	display_state(successor_state,input_data);

	return successor_state;
}

struct State *find_initial_state(struct InputData *input_data)
{
	struct State *initial_state = malloc(sizeof(struct State));
	initial_state->num_of_companies = input_data->num_of_companies;
	initial_state->num_of_coal_blocks = input_data->num_of_coal_blocks;
	initial_state->allocated_coal_blocks = malloc(input_data->num_of_coal_blocks*sizeof(int));
	initial_state->selected_bids_from_companies = malloc(input_data->num_of_companies*sizeof(int));

	for(int i=0;i<initial_state->num_of_coal_blocks;i++)
	{
		initial_state->allocated_coal_blocks[i] = false;
	}

	for(int i=0;i<input_data->num_of_companies;i++)
	{
		if(input_data->bid_data[i]!=NULL)
		{
			if(check_bid_collision(initial_state->allocated_coal_blocks,input_data->bid_data[i][0],input_data->bid_data_dimensions[i][0]))
			{
				initial_state->selected_bids_from_companies[i] = 0;

				for(int j=1;j<input_data->bid_data_dimensions[i][0];j++)
				{
					initial_state->allocated_coal_blocks[input_data->bid_data[i][initial_state->selected_bids_from_companies[i]][j]-1] = true;
				}
			}
			else
			{
				initial_state->selected_bids_from_companies[i] =  no_bid;
			}
		}
		else
			{
				initial_state->selected_bids_from_companies[i] =  no_bid;
			}
	}

	return initial_state;
}

struct State *formulate_goal(struct InputData *input_data)
{	
	struct State *goal_state = malloc(sizeof(struct State));
	goal_state->num_of_companies = input_data->num_of_companies;
	goal_state->num_of_coal_blocks = input_data->num_of_coal_blocks;
	goal_state->selected_bids_from_companies = malloc(input_data->num_of_companies*sizeof(int));
	goal_state->allocated_coal_blocks = malloc(input_data->num_of_coal_blocks*sizeof(int));

	for(int i=0;i<goal_state->num_of_coal_blocks;i++)
	{
		goal_state->allocated_coal_blocks[i] = true;
	}

	for(int i=0;i<input_data->num_of_companies;i++)
	{
		int max = 0;
		int max_index =0;

		for(int j=0;j<input_data->bid_data_dimensions_dimensions[i];j++)
		{
			if(input_data->bid_data[i][j][0]>max)
			{
				max = input_data->bid_data[i][j][0];
				max_index = j;
			}
		}
		if(input_data->bid_data_dimensions_dimensions[i]!=0)
		{
			goal_state->selected_bids_from_companies[i] = max_index;
		}
		else
		{
			goal_state->selected_bids_from_companies[i] = no_bid;
		}
	}

	return goal_state;
}

int is_goal_state(struct State *state,struct State *goal_state)
{
	for(int i=0;i<state->num_of_coal_blocks;i++)
	{
		if(state->allocated_coal_blocks[i]!=goal_state->allocated_coal_blocks[i])
		{
			return false;
		}
	}

	for(int i=0;i<state->num_of_companies;i++)
	{

	}

	return true;
}

/**
* @brief Takes in input from the file name specified.
*
* Example of input file - "1.txt".
* The input is stored in a 3-D array. The state is taken as the total revenue from 
* allocating the coal blocks at that instant. The maximum value of the revenue is 
* the sum of max revenue offered by each company since all other combinations will 
* give revenue less than this amount. This maximum value is taken as the goal state.
* The solution obtained can't reach this value but will be the nearest to it.
* 
* @param file_name the file to take input from
* @return void 
*/
struct InputData *take_input(char file_name[])
{
	FILE *file_ptr;
	file_ptr = fopen(file_name,"r");
	int time, num_of_bids, tmp, num_of_bids_by_company, num_of_coal_blocks_bid_by_company;
	struct InputData *input_data = malloc(sizeof(struct InputData));

	if(file_ptr == NULL)
	{
		printf("%s is empty\n",file_name);
		exit(0);
	}

	fscanf(file_ptr,"%d",&time);
	fscanf(file_ptr,"%d",&input_data->num_of_coal_blocks);
	fscanf(file_ptr,"%d",&num_of_bids);
	fscanf(file_ptr,"%d",&input_data->num_of_companies);

	input_data->bid_data = malloc(input_data->num_of_companies*sizeof(int**));
	input_data->bid_data_dimensions = malloc(input_data->num_of_companies*sizeof(int*));
	input_data->bid_data_dimensions_dimensions = malloc(input_data->num_of_companies*sizeof(int));

	for(int i=0;i<input_data->num_of_companies;i++)
	{
		fscanf(file_ptr,"%d",&tmp);
		fscanf(file_ptr,"%d",&num_of_bids_by_company);
		if(num_of_bids_by_company!=0)
		{
			input_data->bid_data[i] = malloc(num_of_bids_by_company*sizeof(int*));
			input_data->bid_data_dimensions[i] = malloc(num_of_bids_by_company*sizeof(int));
			input_data->bid_data_dimensions_dimensions[i] = num_of_bids_by_company;

			for(int j=0;j<num_of_bids_by_company;j++)
			{
				fscanf(file_ptr,"%d",&tmp);
				fscanf(file_ptr,"%d",&num_of_coal_blocks_bid_by_company);

				input_data->bid_data[i][j] = malloc((num_of_coal_blocks_bid_by_company+1)*sizeof(int));
				input_data->bid_data_dimensions[i][j] = num_of_coal_blocks_bid_by_company + 1;

				for(int k=0;k<num_of_coal_blocks_bid_by_company+1;k++)
				{
					fscanf(file_ptr,"%d",&input_data->bid_data[i][j][k]);
				}
			}
		}
		else
		{
			input_data->bid_data[i] = NULL;
			input_data->bid_data_dimensions[i] = NULL;
			input_data->bid_data_dimensions_dimensions[i] = 0;
		}
	}

	fclose(file_ptr);
	return input_data;
}

void display_input_data(struct InputData *input_data)
{
	printf("\n");

	for(int i=0;i<input_data->num_of_companies;i++)
	{
		for(int j=0;j<input_data->bid_data_dimensions_dimensions[i];j++)
		{
			for(int k=0;k<input_data->bid_data_dimensions[i][j];k++)
			{
				printf("%d ",input_data->bid_data[i][j][k]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

void deallocate_input_data(struct InputData *input_data)
{
	for(int i=0;i<input_data->num_of_companies;i++)
	{
		for(int j=0;j<input_data->bid_data_dimensions_dimensions[i];j++)
		{	
			free(input_data->bid_data[i][j]);		
		}
		if(input_data->bid_data_dimensions[i]!=NULL)
		{
			free(input_data->bid_data_dimensions[i]);
			free(input_data->bid_data[i]);
		}
	}

	free(input_data->bid_data);
	free(input_data->bid_data_dimensions);
	free(input_data->bid_data_dimensions_dimensions);
	free(input_data);
}

/**
* @brief The main function from which all function calls are made.
*
* The main function acts as the problem solving agent. This is because
* all function calls are made from main - formulate_goal, find_initial_state,
* and generate_successor. This was chosen because of the absence of OOP in C.
*
* @param argc The number of command prompt arguments
* @param argv The array containing all the command prompt arguments
* @return 0
*/

int main(int argc, char const *argv[])
{
	int max_iter = 10;
	char file_name[] = "2.txt"; /* an integer value */
	struct InputData *input_data = take_input(file_name);
	display_input_data(input_data);

	// struct State *goal_state = formulate_goal(input_data);
	// printf("cost at goal state = %d\n",cost_heuristic(goal_state,input_data));

	// /* Problem Definition*/
	// struct State *state = find_initial_state(input_data);

	// for(int i=0;i<max_iter;i++)
	// {
	// 	if(is_goal_state(state,goal_state)==true)
	// 	{
	// 		// save_result(state);
	// 		display_state(state,input_data);
	// 		break;
	// 	}
	// 	else
	// 	{	
	// 		state = generate_successor(state,input_data); /* deallocate old state in this function */
	// 	}
	// }

	// // save_result(state);

	// display_state(goal_state,input_data);

	// display_state(state,input_data);

	// deallocate_state(state);
	
	shuffle(input_data);
	deallocate_input_data(input_data);

	

	return 0;
}