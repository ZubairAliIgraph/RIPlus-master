#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <igraph.h>
#include <sys/time.h>
using namespace std;

int create_data_graph(char DataGraphPath[], igraph_t *data_graph);
int create_query_graph(char QueryGraphPath[], igraph_t *query_graph);
int igraph_write_graph_edgelist_query(const igraph_t *graph, FILE *outstream);
int igraph_write_graph_edgelist_data(const igraph_t *graph, FILE *outstream);
void print_int_vector(igraph_vector_int_t *v, FILE *f){
	long int i;
	for (i=0; i<igraph_vector_int_size(v); i++) {
		fprintf(f, " %li", (long int) VECTOR(*v)[i]);
	}
	fprintf(f, "\n");
}
void print_vector(igraph_vector_t *v, FILE *f) {
	long int i;
	for (i=0; i<igraph_vector_size(v); i++) {
		fprintf(f, " %li", (long int) VECTOR(*v)[i]);
	}
	fprintf(f, "\n");
}
int main(int argc, char** argv) 
{ 
	
	/* turn on attribute handling */
 	igraph_i_set_attribute_table(&igraph_cattribute_table);
	igraph_t data_graph,query_graph;
	create_query_graph(argv[1], &query_graph);
	create_data_graph( argv[2], &data_graph);
	printf("number of vertices in data graph  %d\n",(int)igraph_vcount(&data_graph));
	printf("number of edges in data graph  %d\n",(int)igraph_ecount(&data_graph));
	printf("number of vertices in query graph  %d\n",(int)igraph_vcount(&query_graph));
	printf("number of edges in query graph  %d\n",(int)igraph_ecount(&query_graph));
	string str="";
	//str = str + " ./ri36Master ind gfu DataGraphRI.gfu QueryGraphRI.gfu"; // to print all
	//str = str + " ./ri36ToPrint mono gfu DataGraphRI.gfu QueryGraphRI.gfu";
	//str = str + " ./ri36ToCount mono gfu DataGraphRI.gfu QueryGraphRI.gfu";
	//str = str + " ./ri36ToCountAll mono gfu DataGraphRI.gfu QueryGraphRI.gfu";// to count all
	//str = str + " ./ri36ToCountAll mono gfu DataGraphRI.gfu QueryGraphRI.gfu";// to count all
	str = str + " ./ri36ToCountAll mono gfu DataGraphRI.gfu QueryGraphRI.gfu";// to count all
	const char *command = str.c_str();
	cout << "Command to find all solution " << command << endl;
	int x=system(command);
	//cout<<"x"<<x<<endl ;
	//if(x == 31744){
	//	cout<<"Straggler Query"<<endl;//will handle seperatly
		//cout<<"matching time: "<<argv[2]<<endl;
		//cout<<"number of found matches: 0"<<endl;			
	//}
	igraph_destroy(&query_graph);
	igraph_destroy(&data_graph);
	return 0;
}
int create_query_graph(char QueryGraphPath[], igraph_t *query_graph){
	char line[250];
	char ans;
  	FILE *fpw;
	FILE *fp   ;
	igraph_bool_t res;
	igraph_vector_t y,t_nodeLabels;
	//igraph_vector_init(&y,0);
	igraph_vector_init(&t_nodeLabels,0);
	int lineNo=1;
	char ifile[100];
	
		int noOfNodes = 0;
		fp  = fopen( QueryGraphPath, "r"); 
		if(fp == NULL){
			printf("Error! Query Graph File");
			return 0;
		}
		fpw = fopen("queryGraphEdgeList.txt", "w");
		if(fpw == NULL){
			printf("Error! opening queryGraphEdgeList.txt");
			return 0;
	 	}
		while(fgets(line, 255, (FILE*) fp)) {
		lineNo++;
		//printf("\nZubair:%s::" , line);
	  	if(line[0] == 'v'){
			 //geting node labels
		 	 int i;
		 	int j = 0;
		 	int n = strlen(line)-1;
			//printf("\ntest:%c::%d:",line[n], line[n]);
                 	while(line[n] == ' ' || line[n] == 10){
                   		n = n - 1;
                 	}
               
		//printf("\nZaid::%d::", n);
		 i = n ;
		 while(line[i] != ' ')
			 i =  i - 1;
		 i = i + 1;

		 char token[10];

		 //getting query graph number
		 do{
			 token[j] = line[i];
			 j = j + 1;
			 i = i + 1;
		 }while(i <= n);
		 token[j] = '\0';
		 igraph_vector_push_back(&t_nodeLabels,atoi(token));noOfNodes++;
		//printf("\n node = %d label = %d ",noOfNodes-1, t_nodeLabels[noOfNodes-1]);
		//printf("%d",t_nodeLabels[noOfNodes-1]);

	 	}
	 	if(line[0] == 'e'){
		 //geting edge
		 int s, d;
		 int i = 2;
		 int j = 0;
		 char token[6];
		//reading sourse node id
		do{
			token[j] = line[i];
			j = j + 1;
			i = i + 1;
		}while(line[i] != ' ');
		token[j] = '\0';
		s = atoi(token);
		//reading destination node id
		i = i + 1;
		j = 0;
		do{
			token[j] = line[i];
			j = j + 1;
			i = i + 1;
		}while(line[i] != ' ');
		token[j] = '\0';
		d = atoi(token);
         //	printf("%d\t%d\n",d,s);
		fprintf(fpw,"%d %d\n", s, d);
	}
}
		fclose(fp);
		fclose(fpw);
		if ((fpw = fopen("queryGraphEdgeList.txt", "r")) == NULL)
    		{
        		printf("Error! opening queryGraphEdgeList.txt");
        		// Program exits if file pointer returns NULL.
        		exit(1);         
    		}
		igraph_read_graph_edgelist(query_graph, fpw,0, 0) ;
		fclose(fpw);
		igraph_simplify(query_graph, 1, 1, /*edge_comb=*/ 0);
		igraph_is_connected(query_graph, &res, IGRAPH_WEAK); 
  		if (!res) {
    			printf(" query graph not connected1\n");
    			//return 0;
		}
		//printf("number of vertices in query graph  %d\n",(int)igraph_vcount(query_graph));
		//printf("number of edges in query graph  %d\n",(int)igraph_ecount(query_graph));
		SETGAN(query_graph, "vertices", igraph_vcount(query_graph));
		igraph_vector_init_seq(&y, 0, igraph_vcount(query_graph)-1);
		SETVANV(query_graph, "id", &y);
		igraph_vector_destroy(&y);
		for (int i=0; i<igraph_vcount(query_graph); i++){
			if (VAN(query_graph, "id", i) != i){
	      			return 22;
			}
		}
		//print_vector(&t_nodeLabels, stdout); 
		SETVANV(query_graph, "label", &t_nodeLabels);
		for (int i=0; i<igraph_vcount(query_graph); i++) {
			if (VAN(query_graph, "label", i) !=VECTOR(t_nodeLabels)[i]) {
				//printf("node  %d query label %d \n",i,(int)VAN(&query_graph, "label", i))		      ;
				return 23;
			}
		}
		
		igraph_vector_destroy(&t_nodeLabels);
		igraph_integer_t mnd_v_q;
		for (int i=0; i<igraph_vcount(query_graph); i++){
			igraph_vector_init(&y,0);
			igraph_neighbors(query_graph, &y,i, IGRAPH_ALL);	
			igraph_maxdegree(query_graph, &mnd_v_q,igraph_vss_vector(&y), IGRAPH_ALL,0);	
			SETVAN(query_graph,"mnd",i,mnd_v_q);
			if (VAN(query_graph, "mnd", i) != mnd_v_q){
				//printf("mnd not set");
		      		return 2;
				//printf("node  %d mnd %d \n",i,(int)VAN(&query_graph, "mnd", i));
		    	}
			igraph_vector_destroy(&y);
		}  
		igraph_vector_init(&y, 0);
		igraph_degree(query_graph, &y,igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
		SETVANV(query_graph, "deg", &y);
		//return 0;
		igraph_vector_destroy(&y);
		fp = fopen("QueryGraphRI.gfu", "w");
		igraph_write_graph_edgelist_query(query_graph, fp) ;
		fclose(fp);
	return 0;
}
int create_data_graph(char DataGraphPath[],igraph_t *data_graph){
/* turn on attribute handling */
 	igraph_i_set_attribute_table(&igraph_cattribute_table);
	char line[250];
	char ans;
  	FILE *fpw;
	FILE *fp   ;
	igraph_bool_t res;
	
	fp  = fopen(DataGraphPath, "r");
	if(fp == NULL){
		printf("Error! opening DataGraphPath file");
		return 0;
	}
	int lineNo=1;
	igraph_vector_t y,t_nodeLabels;
	//igraph_vector_init(&y,0);
	igraph_vector_init(&t_nodeLabels,0);
	int noOfNodes = 0;
	fpw = fopen("dataGraphEdgeList.txt", "w");
	if(fpw == NULL){
		printf("Error! opening dataGraphEdgeList.txt");
		return 0;
	}
	while(fgets(line, 255, (FILE*) fp)) {
		lineNo++;
	//printf("\nZubair:%s::" , line);
	  if(line[0] == 'v'){
		 //geting node labels
		 int i;
		 int j = 0;
		 int n = strlen(line)-1;
		//printf("\ntest:%c::%d:",line[n], line[n]);
                 while(line[n] == ' ' || line[n] == 10){
                   n = n - 1;
                 }
               
		//printf("\nZaid::%d::", n);
		 i = n ;
		 while(line[i] != ' ')
			 i =  i - 1;
		 i = i + 1;

		 char token[10];

		 //getting query graph number
		 do{
			 token[j] = line[i];
			 j = j + 1;
			 i = i + 1;
		 }while(i <= n);
		 token[j] = '\0';
		igraph_vector_push_back(&t_nodeLabels,atoi(token));noOfNodes++;
		 //VECTOR(t_nodeLabels)[noOfNodes++] = atoi(token);
		//printf("\n node = %d label = %d ",noOfNodes-1, t_nodeLabels[noOfNodes-1]);
		//printf("%d",t_nodeLabels[noOfNodes-1]);

	 }
	 if(line[0] == 'e'){
		 //geting edge
		 int s, d;
		 int i = 2;
		 int j = 0;
		 char token[6];
		//reading sourse node id
		do{
			token[j] = line[i];
			j = j + 1;
			i = i + 1;
		}while(line[i] != ' ');
		token[j] = '\0';
		s = atoi(token);
		//reading destination node id
		i = i + 1;
		j = 0;
		do{
			token[j] = line[i];
			j = j + 1;
			i = i + 1;
		}while(line[i] != ' ');
		token[j] = '\0';
		d = atoi(token);
         //	printf("%d\t%d\n",d,s);
		fprintf(fpw,"%d %d\n", s, d);
	}
}
//exit(0);
		fclose(fp);
		fclose(fpw);
	if ((fpw = fopen("dataGraphEdgeList.txt", "r")) == NULL)
    	{
        printf("Error! dataGraphEdgeList.txt");
        // Program exits if file pointer returns NULL.
        exit(1);         
    	}
	igraph_read_graph_edgelist(data_graph, fpw,0, 0) ;
	//igraph_read_graph_edgelist(data_graph, fpw,82670, 0) ;//for wordnet only.
	fclose(fpw);
	igraph_simplify(data_graph, 1, 1, /*edge_comb=*/ 0);
	igraph_is_connected(data_graph, &res, IGRAPH_WEAK); 
  	if (!res) {
    		//printf(" Daya graph not connected\n");
    		//return 0;
	}
	//printf("number of vertices in Data graph  %d\n",(int)igraph_vcount(data_graph));
	//printf("number of edges in Data graph  %d\n",(int)igraph_ecount(data_graph));
	igraph_vector_init_seq(&y, 0, igraph_vcount(data_graph)-1);
 	SETVANV(data_graph, "id", &y);
	igraph_vector_destroy(&y);
  	for (int i=0; i<igraph_vcount(data_graph); i++){
    		if (VAN(data_graph, "id", i) != i){
     			return 1;
    		}
  	}
	SETVANV(data_graph, "label", &t_nodeLabels);
	for (int i=0; i<igraph_vcount(data_graph); i++){
    		if (VAN(data_graph, "label", i) != VECTOR(t_nodeLabels)[i]){
			//printf("id %d label %d \n",i,(int)VAN(data_graph, "label", i));
	      		return 2;
	    	}
  	}
	igraph_vector_destroy(&t_nodeLabels);
	igraph_integer_t mnd_v_g;
	
	for (int i=0; i<igraph_vcount(data_graph); i++){
		igraph_vector_init(&y,0);
		igraph_neighbors(data_graph, &y,i, IGRAPH_ALL);	
		igraph_maxdegree(data_graph, &mnd_v_g,igraph_vss_vector(&y), IGRAPH_ALL,0);	
		SETVAN(data_graph,"mnd",i,mnd_v_g);
		if (VAN(data_graph, "mnd", i) != mnd_v_g){
			//printf("mnd not set");
	      		return 2;
			//printf("node  %d mnd %d \n",i,(int)VAN(&data_graph, "mnd", i));
	    	}
		igraph_vector_destroy(&y);
	}  
	igraph_vector_init(&y, 0);
	igraph_degree(data_graph, &y,igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
	SETVANV(data_graph, "deg", &y);
	igraph_vector_destroy(&y);
	fp = fopen("DataGraphRI.gfu", "w");
		igraph_write_graph_edgelist_data(data_graph, fp) ;
		fclose(fp);
	return 0;
}

int igraph_write_graph_edgelist_query(const igraph_t *graph, FILE *outstream) {
	
	fprintf(outstream,"#query\n");
	fprintf(outstream,"%d\n",igraph_vcount(graph));
	for(int i=0; i<igraph_vcount(graph); i++){
			fprintf(outstream, "%ld\n",(long int)VAN(graph, "label", i));	
		}
	fprintf(outstream,"%d\n",(int)igraph_ecount(graph));
  	igraph_eit_t it;
  
  	IGRAPH_CHECK(igraph_eit_create(graph, igraph_ess_all(IGRAPH_EDGEORDER_FROM), 
				 &it));
  	IGRAPH_FINALLY(igraph_eit_destroy, &it);

  	while (!IGRAPH_EIT_END(it)) {
    		igraph_integer_t from, to;
    		int ret;
    		igraph_edge(graph, IGRAPH_EIT_GET(it), &from, &to);
    		ret=fprintf(outstream, "%ld %ld\n", 
			(long int) from,
			(long int) to);
    		if (ret < 0) {
      			IGRAPH_ERROR("Write error", IGRAPH_EFILE);
    		}
    	IGRAPH_EIT_NEXT(it);
  	}
  
  igraph_eit_destroy(&it);
  IGRAPH_FINALLY_CLEAN(1);
  return 0;
}

int igraph_write_graph_edgelist_data(const igraph_t *graph, FILE *outstream) {
	
	fprintf(outstream,"#data\n");
	fprintf(outstream,"%d\n",igraph_vcount(graph));
	for(int i=0; i<igraph_vcount(graph); i++){
			fprintf(outstream, "%ld\n",(long int)VAN(graph, "label", i));	
		}
	fprintf(outstream,"%d\n",(int)igraph_ecount(graph));
  	igraph_eit_t it;
  
  	IGRAPH_CHECK(igraph_eit_create(graph, igraph_ess_all(IGRAPH_EDGEORDER_FROM), 
				 &it));
  	IGRAPH_FINALLY(igraph_eit_destroy, &it);

  	while (!IGRAPH_EIT_END(it)) {
    		igraph_integer_t from, to;
    		int ret;
    		igraph_edge(graph, IGRAPH_EIT_GET(it), &from, &to);
    		ret=fprintf(outstream, "%ld %ld\n", 
			(long int) from,
			(long int) to);
    		if (ret < 0) {
      			IGRAPH_ERROR("Write error", IGRAPH_EFILE);
    		}
    	IGRAPH_EIT_NEXT(it);
  	}
  
  igraph_eit_destroy(&it);
  IGRAPH_FINALLY_CLEAN(1);
  return 0;
}


