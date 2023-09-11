#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define DELIMITER ','
#define PARSE_DELIMETERS " ,"
#define EXPRESSION_MAX_LENGTH 1001
#define VARIABLES_MAX 20

typedef char bool;
struct lnode{
	unsigned int id;
	struct lnode * next;
	struct lnode * prev;
};
typedef struct lnode lnodeT;

void list_init(lnodeT **root, unsigned int id){
	lnodeT *newElement = (lnodeT *)malloc( sizeof(lnodeT) );
	newElement->id = id;
	newElement->next = newElement->prev = NULL;
	*root = newElement;
}

void list_insert(lnodeT *root, unsigned int id){
	lnodeT *newElement = (lnodeT *)malloc( sizeof(lnodeT) );
	newElement->id = id;
	newElement->next = newElement->prev = NULL;
	lnodeT * curr = root;
	while(curr->next != NULL){
		if(curr->next->id > newElement->id)
			break;
		curr = curr->next;
	}
	if( curr->next != NULL ){
		curr->next->prev = newElement;
		newElement->next = curr->next;
		newElement->prev = curr;
		curr->next = newElement;
	}
	else{
		newElement->prev = curr;
		curr->next = newElement;
	}
}

lnodeT * list_find(lnodeT *root, unsigned int id){
	lnodeT * curr = root;
	while( curr != NULL ){
		if( curr->id == id ){
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

void list_merge(lnodeT **newRoot, lnodeT *firstRoot, lnodeT *secondRoot ){
	list_init(newRoot, firstRoot->id );
	lnodeT *curr = firstRoot->next;
	while( curr != NULL ){
		list_insert( *newRoot, curr->id );
		curr = curr->next;
	}
	curr = secondRoot;
	while( curr != NULL ){
		list_insert( *newRoot, curr->id );
		curr = curr->next;
	}
}

bool list_equal(lnodeT *firstRoot, lnodeT *secondRoot){
	lnodeT *fNode = firstRoot;
	lnodeT *sNode = secondRoot;
	while( fNode != NULL ){
		if(fNode->id != sNode->id)
			return 0;
		fNode = fNode->next;
		sNode = sNode->next;
	}
	return 1;
}

void list_print(lnodeT *root){
	lnodeT *curr = root;
	while(curr != NULL){
		curr = curr->next;
	}
}

struct mintermGroup{
	char *repr;
	lnodeT* root;
	unsigned int cPosBits;
};
typedef struct mintermGroup mintermGroupT;
int max(int a,int b){
	return a > b ? a : b;
}

void StrReverse(char *s){
	int low = 0;
	int high = strlen(s) - 1;
	while(low < high){
		char c = s[low];
		s[low] = s[high];
		s[high] = c;
		low++;high--;
	}
}

int CompareMintermsByRepr( const void *a , const void * b){
	mintermGroupT *first  = (mintermGroupT *)a;
	mintermGroupT *second = (mintermGroupT *)b;
	if( first->cPosBits == second->cPosBits ){
		return strcmp( first->repr, second->repr );
	}
	return (first->cPosBits - second->cPosBits);
}

int CompareMintermsById( const void * a, const void * b){
	mintermGroupT * first  = (mintermGroupT *)a;
	mintermGroupT * second = (mintermGroupT *)b;
	return ( first->root->id - second->root->id);
}

void CountTerms(char *exp, int *cTerms){
	*cTerms = 1;
	char *ptr = strchr(exp,DELIMITER);
	while(ptr != NULL){
		(*cTerms)++;
		ptr = strchr(ptr + 1, DELIMITER);
	}
}

void ReadInput(int *cMinterms,char *exp_minterms){
	char printfFormat[15];
	sprintf(printfFormat,"%%%d[^\n]s",EXPRESSION_MAX_LENGTH - 1);
	printf("Susunan minterm : ");
	scanf(printfFormat, exp_minterms);
	CountTerms(exp_minterms, cMinterms);
}

void ParseInput(char *exp, mintermGroupT *mt, int cTerms, int *cVariables){
	int i,j;
	int maxMinterm = 0;
	char * pch = strtok(exp, PARSE_DELIMETERS);
	for(i = 0; i < cTerms; i++){
		mt[i].cPosBits = 0;
		mt[i].root = NULL;
		int id = atoi(pch);
		list_init(&mt[i].root, id);
		maxMinterm = max(maxMinterm, id );
		char *binaryRepr = (char *)malloc( (VARIABLES_MAX + 1) * sizeof(char) );
		for(j = 0; j < VARIABLES_MAX; j++){
			if( (id & (1 << j) ) != 0){
				binaryRepr[j] = '1';
				mt[i].cPosBits++;
			}
			else{
				binaryRepr[j] = '0';
			}
		}
		binaryRepr[VARIABLES_MAX] = '\0';
		mt[i].repr = (char *)malloc( (VARIABLES_MAX + 1) * sizeof(char) );
		strcpy(mt[i].repr, binaryRepr);
		if(i < cTerms - 1){
			pch = strtok(NULL, PARSE_DELIMETERS);
		}
	}
	(*cVariables) = 0;
	int binary = 1;
	while( maxMinterm > (binary - 1) ){
		binary = binary * 2;
		(*cVariables)++;
	}
	for(i = 0; i < cTerms; i++){
		mt[i].repr = (char *)realloc( mt[i].repr , (*cVariables + 1) * sizeof(char) );
		mt[i].repr[ *cVariables ] = '\0';
		StrReverse( mt[i].repr );
	}
}

bool CanFormGroup(mintermGroupT firstGroup, mintermGroupT secondGroup, int cVariables){
	int i, bitsDiffer = 0;
	for(i = 0; i < cVariables; i++){
		if( (firstGroup.repr[i] == '-' && secondGroup.repr[i] != '-') ||
			(firstGroup.repr[i] != '-' && secondGroup.repr[i] == '-') ){
				return 0;
		}
		if( firstGroup.repr[i] != secondGroup.repr[i] ){
			bitsDiffer++;
		}
	}
	return (bitsDiffer == 1) ? 1 : 0;
}

void CreateNewGroupRepr(char *newGroupRepr, char *firstGroupRepr, char *secondGroupRepr, int cVariables){
	int i;
	for(i = 0; i < cVariables; i++){
		if( firstGroupRepr[i] == secondGroupRepr[i] ){
			newGroupRepr[i] = firstGroupRepr[i];
		}
		else{
			newGroupRepr[i] = '-';
		}
	}
	newGroupRepr[cVariables] = '\0';
}

void GetPrimeImplicants( mintermGroupT **table, bool ** termsUsed,
						mintermGroupT *primeImplicants, int *lenCol, int cColumns){
	int index = 0;
	int i,j;
	for( i = 0; i <= cColumns; i++){
		for( j = 0; j < lenCol[i]; j++){
			if( !termsUsed[i][j] ){
				primeImplicants[index] = table[i][j];
				index++;
			}
		}
	}
}

void CreatePrimeChart(bool ** primeChart, mintermGroupT* minterms, int cMinterms,
					mintermGroupT * primeImplicants, int cPrimeImplicants){
	int i,j, mintermId;
	lnodeT * curr;
	for(i = 0; i < cPrimeImplicants; i++){
		j = 0;
		curr = primeImplicants[i].root;
		while(curr != NULL){
			mintermId = minterms[j].root->id;
			while( mintermId < (curr->id) ){
				j++;
				mintermId = minterms[j].root->id;
			}
			primeChart[i][j] = 1;
			curr = curr->next;
		}
	}
}

void GetEssentialImplicants(bool ** primeChart, int cPrimeImplicants, int cMinterms, bool * isEssential ){
	int i,j;
	int nextEssential;
	for(j = 0; j < cMinterms; j++){
		nextEssential = -1;
		for(i = 0; i < cPrimeImplicants; i++){
			if( primeChart[i][j] == 1 ){
				if(nextEssential == -1){
					nextEssential = i;
				}
				else{
					nextEssential = -1;
					break;
				}
			}
		}
		if(nextEssential != -1){
			isEssential[nextEssential] = 1;
		}
	}
}

int main(int argc, char *argv[]){
	mintermGroupT *table[VARIABLES_MAX + 1];
	mintermGroupT *primeImplicants;
	mintermGroupT *minterms;
	bool *termsUsed[VARIABLES_MAX + 1];
	bool **primeChart;
	bool *isEssential;
	bool *mintermsCovered;
	int lenCol[VARIABLES_MAX];
	int cVariables;
	int cMinterms;
	int cColumns;
	int cPrimeImplicants;
	char exp_minterms[EXPRESSION_MAX_LENGTH];
	char yt = 'bool';

	do {
	    printf("Masukkan jumlah variabel : \n");
        ReadInput( &cMinterms, exp_minterms);
	table[0] = (mintermGroupT *)malloc( cMinterms * sizeof(mintermGroupT) );
	lenCol[0] = cMinterms;
	minterms = (mintermGroupT *)malloc( cMinterms * sizeof(mintermGroupT) );
	ParseInput(exp_minterms, minterms, cMinterms, &cVariables);
	qsort( minterms, cMinterms, sizeof(mintermGroupT), CompareMintermsById );
	memcpy( table[0], minterms, lenCol[0] * sizeof(mintermGroupT) );
	qsort( table[0], cMinterms, sizeof(mintermGroupT), CompareMintermsByRepr );
	cPrimeImplicants = 0;
	termsUsed[0] = (bool *)calloc( lenCol[0], sizeof(bool) );
	int i,j;
	for(i = 0; i <= cVariables; i++){
		mintermGroupT *nextCol = (mintermGroupT *)malloc( (lenCol[i] * lenCol[i])  * sizeof(mintermGroupT) );
		int nextColPos = 0;
		for( j = 0; j < lenCol[i]; j++){
			int k = j+1;
			while( k < lenCol[i] && table[i][k].cPosBits == table[i][j].cPosBits ){
				k++;
			}
			while( k < lenCol[i] && (table[i][k].cPosBits - table[i][j].cPosBits) == 1){
				if( CanFormGroup( table[i][k], table[i][j], cVariables ) ){
					lnodeT *newGroupRoot;
					list_merge( &newGroupRoot, table[i][j].root, table[i][k].root );
					bool alreadyInColumn = 0;
					int c;
					for( c = 0; c < nextColPos; c++){
						if( list_equal( nextCol[c].root , newGroupRoot ) != 0 ){
							alreadyInColumn = 1;
							break;
						}
					}
					if( !alreadyInColumn ){
						nextCol[nextColPos].root = newGroupRoot;
						nextCol[nextColPos].cPosBits = table[i][j].cPosBits;
						nextCol[nextColPos].repr = (char *)malloc( (cVariables + 1) * sizeof(char) );
						CreateNewGroupRepr( nextCol[nextColPos].repr, table[i][j].repr, table[i][k].repr, cVariables );
						nextColPos++;
					}
					termsUsed[i][j] = termsUsed[i][k] = 1;
				}
				k++;
			}
		}
		lenCol[i+1] = nextColPos;
		for( j = 0; j < lenCol[i]; j++){
			if( !termsUsed[i][j] ){
				cPrimeImplicants++;
			}
		}
		if( lenCol[i+1] == 0 ){
			break;
		}
		table[i+1] = (mintermGroupT *)malloc( lenCol[i+1] * sizeof(mintermGroupT) );
		for( j = 0; j < lenCol[i+1]; j++){
			table[i+1][j] = nextCol[j];
		}
		termsUsed[i+1] = (bool *)calloc( lenCol[i+1], sizeof(bool) );
		free(nextCol);
	}
	cColumns = i;
	primeImplicants = (mintermGroupT *)malloc( cPrimeImplicants * sizeof(mintermGroupT) );
	GetPrimeImplicants( table, termsUsed, primeImplicants, lenCol, cColumns );
	primeChart = (bool **)malloc( cPrimeImplicants * sizeof(bool *) );
	for(i = 0; i < cPrimeImplicants; i++){
		primeChart[i] = (bool *)calloc( cMinterms, sizeof(bool) );
	}
	CreatePrimeChart( primeChart, minterms, cMinterms, primeImplicants, cPrimeImplicants );
	isEssential = (bool *)calloc( cPrimeImplicants, sizeof(bool) );
	GetEssentialImplicants( primeChart, cPrimeImplicants, cMinterms, isEssential );
	mintermsCovered = (bool *)calloc( cMinterms, sizeof(bool) );
	for( i = 0; i < cPrimeImplicants; i++){
		if( isEssential[i] ){
			for( j = 0; j < cMinterms; j++)
				if( primeChart[i][j] )
					mintermsCovered[j] = 1;
		}
	}
	for(j = 0; j <= cColumns; j++){
		for(i = 0; i < lenCol[j]; i++){
			list_print( table[j][i].root );
		}
	}
	for(i = 0; i < cPrimeImplicants; i++){
		list_print( primeImplicants[i].root );
	}
	for(j = 0; j < cMinterms; j++)
	for(j = 0; j < cMinterms; j++)
	for(i = 0; i < cPrimeImplicants; i++){
		for(j = 0; j < cMinterms; j++) {
		}
	}
	printf("Solusi : ");
	for(i = 0; i < cPrimeImplicants; i++){
		if( isEssential[i] ){
		    printf("%s + ", primeImplicants[i].repr);
			list_print( primeImplicants[i].root );
		}
	}
	printf("\nLagi [Y/T] ? ");
	scanf("%c", &yt);
	printf("\n");
	} while (yt=='Y'||yt=='y');

	return 0;
}
