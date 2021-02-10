#include <stdio.h>
#include <stdlib.h>

#include "quadTree.h"

Node* newNode(Point pos, cell* data) {
  
  Node *node = (Node *)malloc(sizeof(Node));
  node->pos = pos; //point where node is located
  node->data = data; //info about cell stored in the node
  return node;

}

Quad* newQuad(Point topL,Point botR) {

  int height = abs(topL.x - botR.x);
  int width = abs(topL.y - botR.y);
  Quad *q = (Quad *)malloc(width*height*(sizeof(Quad)));
  q->topLeft = topL;
  q->botRight = botR;
  q->n = NULL;
  q->topLeftTree  = NULL;
  q->topRightTree = NULL;
  q->botLeftTree  = NULL;
  q->botRightTree = NULL;
  return q;
  
}

void insert(Quad* q, Node* node) {
  
  if(node == NULL) return; //base case
  if(!inBoundary(q->topLeft,q->botRight,node->pos)) return;

  //when we reach a quad of an unit area ; cannot be subdivided further  
  if(abs(q->topLeft.x - q->botRight.x) <= 1 && abs(q->topLeft.y - q->botRight.y) <= 1) {
    if(q->n == NULL)
      q->n = node;
    return;
  }

  //if node is situated on the left side
  if((q->topLeft.x + q->botRight.x)/2 >= node->pos.x) {
    
      //if it is in the top left tree
      if((q->topLeft.y + q->botRight.y)/2 >= node->pos.y) {
	
	if(q->topLeftTree == NULL) {
	  Point p1;
	  Point p2;
	  p1.x = q->topLeft.x;
	  p1.y = q->topLeft.y;
	  p2.x = (q->topLeft.x + q->botRight.x)/2;
	  p2.y = (q->topLeft.y + q->botRight.y)/2 ;
	  q->topLeftTree = newQuad(p1,p2);
	}
	
	insert(q->topLeftTree,node);
	
      } else { //if it is at the bottom left tree
	
	if(q->botLeftTree == NULL) {
	  Point p1;
	  Point p2;
	  p1.x = q->topLeft.x;
	  p1.y = (q->topLeft.y + q->botRight.y)/2;
	  p2.x = (q->topLeft.x + q->botRight.x)/2;
	  p2.y = q->botRight.y;
	  q->botLeftTree = newQuad(p1,p2);
	}
	
	insert(q->botLeftTree,node);
	
      }
      
  } else { //if node is situated at the right side
    
    //if it is at the top right tree
    if((q->topLeft.y + q->botRight.y)/2 >= node->pos.y) {
      
      if(q->topRightTree == NULL) {
	Point p1;
	Point p2;
	p1.x = (q->topLeft.x + q->botRight.x)/2;
	p1.y = q->topLeft.y;
	p2.x = q->botRight.x;
	p2.y = (q->topLeft.y + q->botRight.y)/2;
	q->topRightTree = newQuad(p1,p2);
      }
      
      insert(q->topRightTree,node);
      
    } else {
      
      if(q->botRightTree == NULL) { //if it is at the bottom right tree
	Point p1;
	Point p2;
	p1.x = (q->topLeft.x + q->botRight.x)/2;
	p1.y = (q->topLeft.y + q->botRight.y)/2;
	p2.x = q->botRight.x;
	p2.y = q->botRight.y;
	q->botRightTree = newQuad(p1,p2);
      }
      
      insert(q->botRightTree,node);

    }
  } 
}

Node* search(Quad* q,Point p) {
  if(!inBoundary(q->topLeft, q->botRight, p)) return NULL;
  
  if(q->n != NULL) //base case 
    return q->n;
  
  if((q->topLeft.x + q->botRight.x)/2 >= p.x) {

    if((q->topLeft.y + q->botRight.y)/2 >= p.y) { //if it is top left tree
      if(q->topLeftTree == NULL) {
	return NULL;
      }
      
      return search(q->topLeftTree,p);
      
    } else {
      
      if(q->botLeftTree == NULL) { //if it is the bottom right tree
	return NULL;
      }
      
      return search(q->botLeftTree,p);
      
    }
  } else {
    
    if((q->topLeft.y + q->botRight.y)/2 >= p.y) { //if it is at the top right tree
      
      if(q->topRightTree == NULL) {
	return NULL;
      }
      
      return search(q->topRightTree,p);
      
    } else {
      
      if(q->botRightTree == NULL) { //if it at the bottom right tree
	return NULL;
      }

      return search(q->botRightTree,p);

    }
  } 
}

int inBoundary(Point topL, Point botR, Point p) {
  return (p.x >= topL.x && p.x <= botR.x && p.y >= topL.y && p.y <= botR.y);
}

void quadtree_free(Quad* q) {
  
  if(q->n == NULL) free(q);
  else {
      free(q->n);
      if(q->topLeftTree != NULL)
	quadtree_free(q->topLeftTree);
      else if(q->topRightTree != NULL)
	quadtree_free(q->topRightTree);
      else if(q->botLeftTree != NULL)
	quadtree_free(q->botLeftTree);
      else if(q->botRightTree != NULL)
	quadtree_free(q->botRightTree);
  }
  
}
