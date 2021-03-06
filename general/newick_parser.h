#ifndef DTRIPLET_NEWICK_PARSER_H
#define DTRIPLET_NEWICK_PARSER_H

#include "tree_component.h"
#include <string>
#include <fstream>
using namespace std;

/*
 *
 *
 * Reading and writing for T1. We store T1 in a pre order layout.
 *
 *
 */

void T1read(const char* file){

    string T1newick;
    ifstream fin;
    fin.open(file, ios_base::in);

    //read newick string of T1
    fin>>T1newick;
    fin.close();
    /*
     *
     * Find the number of nodes in T1 by scanning the newick description of T1
     * O(N/B) I/Os
     *
     */
    int i;
    int up = T1newick.size() - 1;
    T1size = 0;
    T1numLeafs = 0;
    i = 0;
    while(i<up){
        if(T1newick[i] == '('){
            //internal node
            i++;
            T1size++;
        }
        else if(T1newick[i] == ',' || T1newick[i] == ')') {
            i++;
        }
        else{
            //leaf
            T1size++;
            T1numLeafs++;
            i++;
            while(T1newick[i] >= '0' && T1newick[i] <= '9'){
                i++;
            }
        }
    }

    /*
     *
     * we know the number of nodes, so let's just initialize the memory for T1
     *
     *
     */

    T1 = new tree_node[T1size];

    /*
     * create the connections in T1, following the pre order layout
     * O(N/B) I/Os
     *
     */
/*

    if(T1size%2 == 0){
        T1numLeafs = (T1size+2)/2;
    }
    else{
        T1numLeafs = (T1size+1)/2;
    }
*/

    int curInternalID = T1numLeafs;

    int *myConnectionStack = new int[T1size];
    int myConnectionStackTop = 0;
    int *degreeStack = new int[T1size];
    int degreeStackTop = 0;

    i = 0;
    int curNode = 0;
    while(i<T1newick.size()){

        if(T1newick[i] == '('){
            T1[curNode].id = curInternalID++;
            myConnectionStack[myConnectionStackTop++] = curNode;
            if(degreeStackTop > 0)
                degreeStack[degreeStackTop-1]++;
            degreeStack[degreeStackTop++] = 0;
            i++;
            curNode++;
        }
        else if(T1newick[i] == ')'){

            int degree = degreeStack[degreeStackTop-1];
            int parentIndex = myConnectionStackTop-degree-1;
            int toMergePos =  myConnectionStack[parentIndex];

            for(int d=1;d<=degree;d++)
                T1[toMergePos].children.push_back(myConnectionStack[parentIndex+d]);

            myConnectionStackTop-=degree;
            degreeStackTop--;
            i++;
        }
        else if(T1newick[i] >= '0' && T1newick[i] <= '9'){

            char treeLabel[20]; //buffer used for the label
            int j = 0;
            while(T1newick[i] >= '0' && T1newick[i] <= '9'){
                treeLabel[j] = T1newick[i];
                i++;
                j++;
            }

            treeLabel[j] = '\0';
            T1[curNode].id = atoi(treeLabel)-1;
            myConnectionStack[myConnectionStackTop++] = curNode;
            degreeStack[degreeStackTop-1]++;
            curNode++;

        }
        else{
            i++;
        }
    }

    delete[] myConnectionStack;
    delete[] degreeStack;
}

struct lpStackNode{
    int degree, finishedProcessing;
};

void T1write(const char* file){

    string outputString;

    //the most recent value in a stack will correspond to an internal node in the tree. We need to know what is the degree of the node and how many of the subtrees have we already
    //finished processing.

    lpStackNode * lpStack = new lpStackNode[T1size];
    int lpStackTop = 0;
    //for every internal node I need a `(`, id, `)` and a `,`. For every leaf I need an id.
    //so a lower bound for the number of characters is 4*numInternalNodes + numLeafs
    outputString.reserve(4*(T1size - T1numLeafs) + T1numLeafs);
    int i,j;
    for(i=0;i<T1size;i++){

        if(T1[i].children.size()>0){
            //we are at an internal node
            lpStack[lpStackTop].degree = T1[i].children.size();
            lpStack[lpStackTop++].finishedProcessing = 0;
            outputString.push_back('(');
            continue;
        }
        else
        {
            //we are at a leaf
            outputString.append(to_string(T1[i].id + 1));

        }

        j = lpStackTop-1;
        while(j>=0 && lpStack[j].finishedProcessing == lpStack[j].degree - 1){
            outputString.push_back(')');
            j--;
        }

        lpStackTop = j+1;

        if(lpStackTop > 0){
            lpStack[lpStackTop-1].finishedProcessing++;
            outputString.push_back(',');
        }

    }


    delete[] lpStack;

    outputString.push_back(';');

    ofstream fout;
    fout.open(file, ios_base::out);
    fout<<outputString<<endl;
    fout.close();

}

 /*
 *
 * Reading and writing for T2. We store T2 in a post order layout.
 *
 *
 */

void T2read(const char* file){


    string T2newick;
    ifstream fin;
    fin.open(file, ios_base::in);

    //read newick string of T2
    fin>>T2newick;
    fin.close();

    /*
     *
     * Find the number of nodes in T2 by scanning the newick description of T2
     * O(N/B) I/Os
     *
    */

    int i;
    int up = T2newick.size() - 1;
    T2size = 0;
    T2numLeafs = 0;
    i = 0;
    while(i<up){
        if(T2newick[i] == '('){
            //internal node
            i++;
            T2size++;
        }
        else if(T2newick[i] == ',' || T2newick[i] == ')') {
            i++;
        }
        else{
            //leaf
            T2size++;
            T2numLeafs++;
            i++;
            while(T2newick[i] >= '0' && T2newick[i] <= '9'){
                i++;
            }
        }
    }

    /*
     *
     * we know the number of nodes, so let's just initialize the memory for T2
     *
     *
    */

    T2 = new tree_node[T2size];

     /*
      * create the connections in T2, following the post order layout
      * O(N/B) I/Os
     */


    int curInternalID = T2numLeafs;

    int *myConnectionStack = new int[T2size];
    int myConnectionStackTop = 0;
    int * degreeStack = new int[T2size];
    int degreeStackTop = 0;

    i = 0;
    int curNode = 0;
    while(i<T2newick.size()){

        if(T2newick[i] == ')'){

            int degree = degreeStack[degreeStackTop-1];
            for(int d=1;d<=degree;d++) {
                T2[curNode].children.push_back(myConnectionStack[myConnectionStackTop - d]);
            }
            T2[curNode].id = curInternalID++;
            myConnectionStack[myConnectionStackTop-degree] = curNode++;
            myConnectionStackTop = myConnectionStackTop - degree + 1;
            degreeStackTop--;
            i++;

        }
        else if(T2newick[i] >= '0' && T2newick[i] <= '9'){

            char treeLabel[20]; //buffer used for the label
            int j = 0;
            while(T2newick[i] >= '0' && T2newick[i] <= '9'){
                treeLabel[j] = T2newick[i];
                i++;
                j++;
            }

            treeLabel[j] = '\0';
            T2[curNode].id = atoi(treeLabel)-1;
            degreeStack[degreeStackTop-1]++;
            myConnectionStack[myConnectionStackTop++] = curNode++;

        }
        else if(T2newick[i] == '('){
            if(degreeStackTop > 0)
                degreeStack[degreeStackTop-1]++;
            degreeStack[degreeStackTop++] = 0;
            i++;
        }
        else{
            i++;
        }

    }

    delete[] myConnectionStack;
    delete[] degreeStack;

}

void T2write(const char* file){

    string outputString;

    //the most recent value in a stack will correspond to an internal node in the tree. We need to know what is the degree of the node and how many of the subtrees have we already
    //finished processing.

    lpStackNode * lpStack = new lpStackNode[T2size];
    int lpStackTop = 0;
    //for every internal node I need a `(`, id, `)` and a `,`. For every leaf I need an id.
    //so a lower bound for the number of characters is 4*numInternalNodes + numLeafs
    outputString.reserve(4*(T2size - T2numLeafs) + T2numLeafs);
    int i,j;
    for(i=T2size-1;i>=0;i--){

        if(T2[i].children.size() > 0){
            //we are at an internal node
            lpStack[lpStackTop].degree = T2[i].children.size();
            lpStack[lpStackTop++].finishedProcessing = 0;
            outputString.push_back('(');
            continue;
        }
        else
        {
            //we are at a leaf
            outputString.append(to_string(T2[i].id + 1));

        }

        j = lpStackTop-1;
        while(j>=0 && lpStack[j].finishedProcessing == lpStack[j].degree - 1){
            outputString.push_back(')');
            j--;
        }

        lpStackTop = j+1;

        if(lpStackTop > 0){
            lpStack[lpStackTop-1].finishedProcessing++;
            outputString.push_back(',');
        }

    }


    delete[] lpStack;

    outputString.push_back(';');

    ofstream fout;
    fout.open(file, ios_base::out);
    fout<<outputString<<endl;
    fout.close();

}

#endif //DTRIPLET_NEWICK_PARSER_H
