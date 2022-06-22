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
int CandidateRegionFinding(igraph_t *data_graph,igraph_t *query_graph,long int *pivot_node, long int *pivot_node_eccentricity, igraph_vector_int_t *OrderedPivotVertexMatch);
int PivotVertexSelection(igraph_t *data_graph,igraph_t *query_graph, long int *pivot_node, long int *pivot_node_eccentricity, igraph_vector_int_t *ThreeleastmatchVertex);
int NhbdVerify(igraph_integer_t *res, igraph_vector_int_t *v,igraph_t *data_graph, igraph_t *query_graph);

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
void print_edges(const igraph_t *graph) {
    long ecount = igraph_ecount(graph);
    long i;
    for (i = 0; i < ecount; ++i) {
        printf("%d %d\n", IGRAPH_FROM(graph, i), IGRAPH_TO(graph, i));
    }
    printf("\n");
}
int comp(void *extra, const void *a, const void *b) {
	igraph_vector_t *v=(igraph_vector_t*) extra;
	int *aa=(int*) a;
	int *bb=(int*) b;
	igraph_real_t aaa=VECTOR(*v)[*aa];
	igraph_real_t bbb=VECTOR(*v)[*bb];
  
	if (aaa < bbb) { 
		return -1;
	} else if (aaa > bbb) { 
		return 1;
	}
  
	return 0;
}
bool isSubSequenceDeg(igraph_vector_t *QueryVertexDegSeq, igraph_vector_t *DataVertexDegSeq, int m, int n) 
{ 
    // Base Cases 
    if (m == 0) return true; 
    if (n == 0) return false; 
  
    // If last characters of two strings are matching 
	if(VECTOR(*QueryVertexDegSeq)[m-1] <= VECTOR(*DataVertexDegSeq)[n-1])
        return isSubSequenceDeg(QueryVertexDegSeq, DataVertexDegSeq, m-1, n-1); 
    // If last characters are not matching 
	return isSubSequenceDeg(QueryVertexDegSeq, DataVertexDegSeq, m, n-1); 
} 
bool isSubSequenceLabel(igraph_vector_t *QueryVertexLabelSeq, igraph_vector_t *DataVertexLabelSeq, int m, int n) 
{ 
    // Base Cases 
    if (m == 0) return true; 
    if (n == 0) return false; 
  
    // If last characters of two strings are matching 
	if(VECTOR(*QueryVertexLabelSeq)[m-1] == VECTOR(*DataVertexLabelSeq)[n-1])
        return isSubSequenceLabel(QueryVertexLabelSeq, DataVertexLabelSeq, m-1, n-1); 
    // If last characters are not matching 
	return isSubSequenceLabel(QueryVertexLabelSeq, DataVertexLabelSeq, m, n-1); 
} 

int count=0;
int EmbCountlimit=0;
igraph_adjlist_t CandidateMatchlist;	  
int main(int argc, char** argv) 
{ 
	struct timeval t0;
  	struct timeval t1;
  	float elapsed_msec=0;
	long int pivot_node,pivot_node_eccentricity;
	igraph_vector_int_t OrderedPivotVertexMatch, ThreeleastmatchVertex;

	/* turn on attribute handling */
 	igraph_i_set_attribute_table(&igraph_cattribute_table);
	igraph_t data_graph,query_graph;
	EmbCountlimit= atoi(argv[1]);
	create_data_graph( argv[2], &data_graph);
	create_query_graph(argv[1], &query_graph);
	printf("number of vertices in data graph  %d\n",(int)igraph_vcount(&data_graph));
	printf("number of edges in data graph  %d\n",(int)igraph_ecount(&data_graph));
	printf("number of vertices in query graph  %d\n",(int)igraph_vcount(&query_graph));
	printf("number of edges in query graph  %d\n",(int)igraph_ecount(&query_graph));
	igraph_vector_int_init(&ThreeleastmatchVertex, 3);
	
	int test=PivotVertexSelection(&data_graph,&query_graph,&pivot_node,&pivot_node_eccentricity, &ThreeleastmatchVertex);
	if(test==1){
		printf("Pivot not found\n");
		igraph_vector_int_destroy(&ThreeleastmatchVertex);
		igraph_destroy(&query_graph);
		igraph_destroy(&data_graph);
		return 0;
		
	}
	igraph_vector_int_init(&OrderedPivotVertexMatch, 0);
	CandidateRegionFinding(&data_graph, &query_graph, &pivot_node, &pivot_node_eccentricity, &OrderedPivotVertexMatch);
	printf("Sorted id of centre of candidate regions as per size");
	print_int_vector(&OrderedPivotVertexMatch, stdout);
	for(int i=0; i<igraph_vector_int_size(&OrderedPivotVertexMatch); i++){//Varies over Candidate Regions.
		printf("Call for CandidateRegionExploration %d\n", VECTOR(OrderedPivotVertexMatch)[i]);
		igraph_vector_ptr_t ptr_region;
		igraph_vector_ptr_init(&ptr_region, 0);	
		igraph_neighborhood(&data_graph, &ptr_region,igraph_vss_1(VECTOR(OrderedPivotVertexMatch)[i]), pivot_node_eccentricity, IGRAPH_ALL);
		igraph_vector_t visited_node;
		igraph_vector_init(&visited_node, 0);
		igraph_vector_copy(&visited_node,(igraph_vector_t*)VECTOR(ptr_region)[0]);
		igraph_vector_ptr_clear(&ptr_region);
		igraph_t CandidateRegion;
		igraph_vs_t vs;
		igraph_vs_vector(&vs,&visited_node);
		igraph_induced_subgraph(&data_graph, &CandidateRegion, vs, IGRAPH_SUBGRAPH_AUTO);
		printf("number of vertices in CandidateRegion  %d\n",(int)igraph_vcount(&CandidateRegion));
		printf("number of edges in CandidateRegion  %d\n",(int)igraph_ecount(&CandidateRegion));
		if(igraph_vcount(&query_graph) <= igraph_vcount(&CandidateRegion) && igraph_ecount(&query_graph) <= igraph_ecount(&CandidateRegion))	{
			printf("Volume of candidate region OK\n");
			FILE *fp = fopen("CandidateRegionRI.gfu", "w");
			igraph_write_graph_edgelist_data(&CandidateRegion, fp) ;
			fclose(fp);
			string str="";
			//str = str + "s ./ri36Master mono gfu CandidateRegionRI.gfu QueryGraphRI.gfu";// to print all
			//str = str + "s ./ri36ToPrint mono gfu CandidateRegionRI.gfu QueryGraphRI.gfu";
			//str = str + "s ./ri36ToCount mono gfu CandidateRegionRI.gfu QueryGraphRI.gfu";
			//str = str + "s ./ri36ToCountAll mono gfu CandidateRegionRI.gfu QueryGraphRI.gfu";// to count all
			//str = str + "./ri36ToCountAll mono gfu CandidateRegionRI.gfu QueryGraphRI.gfu";// to count all
			str = str + "./ri36ToCountAll ind gfu CandidateRegionRI.gfu QueryGraphRI.gfu";// to count all
			const char *command = str.c_str();
			cout << "Command to find all solution " << command << endl;
			int x=system(command);
			//cout<<"x"<<x<<endl ;
			//if(x == 31744){
			//	cout<<"Straggler Query"<<endl;//will handle seperatly
			//	cout<<"matching time: "<<argv[2]<<endl;
			//	cout<<"number of found matches: 0"<<endl;			
			//}
			igraph_vector_destroy(&visited_node);
			igraph_vs_destroy(&vs);
			igraph_destroy(&CandidateRegion);
			continue;
		}
		printf("Volume of candidate region NOT OK\n");
		igraph_vector_destroy(&visited_node);
		igraph_vs_destroy(&vs);
		igraph_destroy(&CandidateRegion);
	}

	igraph_vector_int_destroy(&ThreeleastmatchVertex);
	igraph_vector_int_destroy(&OrderedPivotVertexMatch);
	igraph_destroy(&query_graph);
	igraph_destroy(&data_graph);
	return 0;
}
int CandidateRegionFinding(igraph_t *data_graph,igraph_t *query_graph,long int *pivot_node, long int *pivot_node_eccentricity, igraph_vector_int_t *OrderedPivotVertexMatch){
igraph_vector_ptr_t ptr_region;
igraph_vector_t visited_node, CandidateRegionSize;
igraph_vector_int_t *pivot_node_match;
igraph_vector_init(&CandidateRegionSize, 0);
 	pivot_node_match=igraph_adjlist_get(&CandidateMatchlist,*pivot_node);
	for(int i=0; i<igraph_vector_int_size(pivot_node_match); i++){//Varies over Candidate Regions.
		igraph_vector_ptr_init(&ptr_region, 0);
		igraph_vector_init(&visited_node, 0);
		igraph_neighborhood(data_graph, &ptr_region,igraph_vss_1(VECTOR(*pivot_node_match)[i]),*pivot_node_eccentricity, IGRAPH_ALL);
		//printf("%dth region pivot match %d \n", i,VECTOR(*pivot_node_match)[i]);
		//printf("%dth region size %ld \n ",i, igraph_vector_ptr_size(&ptr_region));
		//VECTOR(ptr_region)[0];
		//print_vector((igraph_vector_t*)VECTOR(ptr_region)[0], stdout); 
		igraph_vector_copy(&visited_node,(igraph_vector_t*)VECTOR(ptr_region)[0]);
		igraph_vector_ptr_clear(&ptr_region);
		//printf("#V(CandidateRegion) %d\n",(int)igraph_vector_size(&visited_node));
		
		//printf("S.No. %d NodeId %d #V(R) %d\n", i, (int )VECTOR(*pivot_node_match)[i], (int)igraph_vector_size(&visited_node));
		igraph_t CandidateRegion;
		igraph_vs_t vs;
		igraph_vs_vector(&vs,&visited_node);
		igraph_induced_subgraph(data_graph, &CandidateRegion, vs, IGRAPH_SUBGRAPH_AUTO);
		//printf("number of vertices in CandidateRegion  %d\n",(int)igraph_vcount(&CandidateRegion));
		//printf("number of edges in CandidateRegion  %d\n",(int)igraph_ecount(&CandidateRegion));
		int ecount=igraph_ecount(&CandidateRegion); int vcount=igraph_vcount(&CandidateRegion);
		igraph_real_t avg_path_len=0;
		igraph_average_path_length(&CandidateRegion, &avg_path_len, IGRAPH_UNDIRECTED, 1);
		// printf("Length of the average shortest paths: %f\n", (float) avg_path_len); 
		//printf("ecount %d vcount %d\n", ecount, vcount);
		//igraph_vector_push_back(&CandidateRegionSize,  ecount * vcount);//volume based ordering
		//igraph_vector_push_back(&CandidateRegionSize,  vcount);//size based ordering
		//igraph_vector_push_back(&CandidateRegionSize,  ecount);//edge count based ordering
		igraph_vector_push_back(&CandidateRegionSize,  avg_path_len);//avg path length based ordering
		//igraph_vector_push_back(&CandidateRegionSize, 2*ecount/ vcount*(vcount-1));//density based ordering
		igraph_destroy(&CandidateRegion);
		igraph_vector_destroy(&visited_node);
		igraph_vs_destroy(&vs);
		
	}
	//printf("CandidateRegion Size ");print_vector(&CandidateRegionSize, stdout); 
	igraph_vector_int_t idx;
	igraph_vector_int_init_seq(&idx, 0, igraph_vector_int_size(pivot_node_match)) ;
	igraph_qsort_r(VECTOR(idx), igraph_vector_int_size(pivot_node_match), sizeof(VECTOR(idx)[0]), (void*) &CandidateRegionSize, comp);
	//cout<<"Sorted Candidate Regions"<<endl;
	for (int i = 0; i < igraph_vector_int_size(pivot_node_match); i++) {
		igraph_vector_int_push_back(OrderedPivotVertexMatch, VECTOR(*pivot_node_match)[VECTOR(idx)[i]]);
        	//printf("S.No. %d Candidate Region Centered to Vertex id %d of size %g\n ", i, (int)VECTOR(*pivot_node_match)[VECTOR(idx)[i]], VECTOR(CandidateRegionSize)[ VECTOR(idx)[i] ]);
    		}
	igraph_vector_destroy(&CandidateRegionSize);
	igraph_vector_int_destroy(&idx);
	return 0;

}
int PivotVertexSelection(igraph_t *data_graph,igraph_t *query_graph, long int *pivot_node, long int *pivot_node_eccentricity, igraph_vector_int_t *ThreeleastmatchVertex){

igraph_vector_t v ;
igraph_vector_int_t Vid;

igraph_vector_init(&v, igraph_vcount(query_graph));
igraph_vector_int_init(&Vid, igraph_vcount(query_graph));
igraph_adjlist_init_empty(&CandidateMatchlist, igraph_vcount(query_graph));

	for (int i=0; i<igraph_vcount(query_graph); i++){
		for (int j=0; j<igraph_vcount(data_graph); j++){
			if(VAN(query_graph, "label", i) == VAN(data_graph, "label", j) && VAN(query_graph, "deg", i) <= VAN(data_graph, "deg", j) && VAN(query_graph, "mnd", i) <= VAN(data_graph, "mnd", j)){

				igraph_vector_int_push_back(igraph_adjlist_get(&CandidateMatchlist,i), j);
			}
		}
		//printf("Vertex id %d match in data graph  ",i);
		//print_int_vector(igraph_adjlist_get(&CandidateMatchlist,i), stdout);
		VECTOR(Vid)[i] = i;
		VECTOR(v)[i] = igraph_vector_int_size(igraph_adjlist_get(&CandidateMatchlist,i));
		if(VECTOR(v)[i]==0){
			printf("Match not found due to query node %d;",i);
			igraph_vector_int_destroy(&Vid);
			igraph_vector_destroy(&v); 
			return 1;
		}
	}
	igraph_qsort_r(VECTOR(Vid), igraph_vcount(query_graph), sizeof(VECTOR(Vid)[0]), (void*) &v, comp);
	//cout<<"Sorted Query Graph Vertices"<<endl;
	//for (int i=0; i<igraph_vcount(query_graph); i++) { 
	//	printf("vetex id %d ", VECTOR(Vid)[i]);
    	//	printf("#Match %g ", VECTOR(v)[ VECTOR(Vid)[i] ]);
	//	printf("\n");
 	//}
	igraph_vector_destroy(&v); 
	igraph_vector_t	eccentricity, score, query_node_eccentricity;
	igraph_vector_init(&score, 0);
	igraph_vector_init(&query_node_eccentricity, 0);
	for (int i=0; i<3; i++) {
		VECTOR(*ThreeleastmatchVertex)[i]=VECTOR(Vid)[i];
		igraph_vector_init(&eccentricity, 1);
		igraph_eccentricity(query_graph,&eccentricity,igraph_vss_1((igraph_integer_t)VECTOR(Vid)[i]), IGRAPH_ALL);
		//printf("Ecentricity of vertex %d is ", VECTOR(Vid)[i]);
		//print_vector(&eccentricity, stdout);
		NhbdVerify(&VECTOR(Vid)[i], igraph_adjlist_get(&CandidateMatchlist,VECTOR(Vid)[i]), data_graph, query_graph);
		//printf("after Nhbdverify call");
		//printf("size %ld \n", igraph_vector_int_size(igraph_adjlist_get(&CandidateMatchlist,VECTOR(Vid)[i])));
		//print_int_vector(igraph_adjlist_get(&CandidateMatchlist,VECTOR(Vid)[i]), stdout);
		if(igraph_vector_int_size(igraph_adjlist_get(&CandidateMatchlist,VECTOR(Vid)[i])) * VECTOR(eccentricity)[0]==0){
			printf("pivot node not exist;\n");	
			igraph_vector_int_destroy(&Vid);
			igraph_vector_destroy(&score);	
		 	return 1;
		}
		igraph_vector_push_back(&score, igraph_vector_int_size(igraph_adjlist_get(&CandidateMatchlist,VECTOR(Vid)[i])) * VECTOR(eccentricity)[0]);
		igraph_vector_push_back(&query_node_eccentricity, VECTOR(eccentricity)[0]);
		igraph_vector_destroy(&eccentricity);
	}
	*pivot_node=VECTOR(Vid)[igraph_vector_which_min(&score)];
	*pivot_node_eccentricity=VECTOR(query_node_eccentricity)[igraph_vector_which_min(&score)];
	//printf("Score are:");
	//print_vector(&score, stdout);
	printf("Pivot Node in Query Graph %ld\n", *pivot_node);
	printf("Pivot Node's Ecc in Query Graph %ld\n", *pivot_node_eccentricity);

	igraph_vector_int_destroy(&Vid);
	igraph_vector_destroy(&score);
	return 0;
}

int NhbdVerify(igraph_integer_t *v_q, igraph_vector_int_t *v,igraph_t *data_graph, igraph_t *query_graph){
	igraph_vector_t adj_v_q,adj_v_g, label_adj_v_q, label_adj_v_g, deg_adj_v_q, deg_adj_v_g;
	igraph_vector_int_t y;
	igraph_vector_int_init(&y,0);
	igraph_vector_init(&adj_v_q, 0);
	igraph_vector_init(&label_adj_v_q, 0);
	igraph_vector_init(&deg_adj_v_q, 0);
	igraph_neighbors(query_graph, &adj_v_q,*v_q, IGRAPH_ALL);
	igraph_cattribute_VANV(query_graph, "label",igraph_vss_vector(&adj_v_q), &label_adj_v_q);
	igraph_cattribute_VANV(query_graph, "deg",igraph_vss_vector(&adj_v_q), &deg_adj_v_q);
	igraph_vector_sort(&label_adj_v_q);
	igraph_vector_sort(&deg_adj_v_q);
	//printf("neighbour");
	//print_vector(&adj_v_q, stdout);
	igraph_vector_destroy(&adj_v_q);
	for(int i=0; i<igraph_vector_int_size(v); i++){
		igraph_vector_init(&adj_v_g, 0);
		igraph_vector_init(&label_adj_v_g, 0);
		igraph_vector_init(&deg_adj_v_g, 0);
		igraph_neighbors(data_graph, &adj_v_g,VECTOR(*v)[i], IGRAPH_ALL);
		igraph_cattribute_VANV(data_graph, "label",igraph_vss_vector(&adj_v_g), &label_adj_v_g);igraph_vector_sort(&label_adj_v_g); 
		igraph_cattribute_VANV(data_graph, "deg",igraph_vss_vector(&adj_v_g), &deg_adj_v_g);igraph_vector_sort(&deg_adj_v_g); 
		//printf("neighbour_data");
		//print_vector(&adj_v_g, stdout);
		//printf("neighbour_query_label");
		//print_vector(&label_adj_v_q, stdout);
		//printf("neighbour_date_label");
		//print_vector(&label_adj_v_g, stdout);
		//printf("neighbour_query_degree");
		//print_vector(&deg_adj_v_q, stdout);
		//printf("neighbour_data_degree");
		//print_vector(&deg_adj_v_g, stdout);
		if(isSubSequenceDeg(&deg_adj_v_q, &deg_adj_v_g, igraph_vector_size(&deg_adj_v_q), igraph_vector_size(&deg_adj_v_g)) && isSubSequenceLabel(&label_adj_v_q, &label_adj_v_g, igraph_vector_size(&label_adj_v_q), igraph_vector_size(&label_adj_v_g))){
			//printf(" YES ");
			igraph_vector_int_push_back(&y,VECTOR(*v)[i]);		
		}else {
			//printf(" NO ");
		}

		igraph_vector_destroy(&adj_v_g);
		igraph_vector_destroy(&label_adj_v_g);
		igraph_vector_destroy(&deg_adj_v_g);
	}
	igraph_vector_int_update(v,&y);
	//printf("after removal");printf("size %ld \n", igraph_vector_int_size(v));
	//print_int_vector(v, stdout);	
	igraph_vector_destroy(&label_adj_v_q);
	igraph_vector_destroy(&deg_adj_v_q);
	igraph_vector_int_destroy(&y);
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

  


