#include <stdio.h>
#include <stdlib.h>

typedef struct poly_term_node polynode;

struct poly_term_node {
	int deg;
	double coeff;
	struct poly_term_node *next;
};

int num_polys = 0;
polynode *polypool[10] = {NULL};

polynode * create_node(int degree, double coeff) {
	polynode *p;
	if ( (p = (polynode*)malloc(sizeof(polynode))) == NULL ) {
		printf("Error: cannot allocate poly node.\n");
		exit(EXIT_FAILURE);
	}
	p->deg = degree;
	p->coeff = coeff;
	p->next = NULL;
}

void attach_term(int poly_id, int degree, double coeff) {
	polynode *p = create_node(degree, coeff);
	p->next = polypool[poly_id];
	polypool[poly_id] = p;
}

void accept() {
	char ch;
	int deg;
	double coeff;
	do {
		printf("Enter degree:\t"); scanf("%d", &deg);
		printf("Enter coeff:\t"); scanf("%lf", &coeff);
		attach_term(num_polys, deg, coeff);
		printf("Continue? y/n:\t"); scanf(" %c", &ch);
	} while (ch != 'n');
	num_polys++;
}

void remove_poly() {

}

void add(int poly1, int poly2) {
	polynode *ptr1 = polypool[poly1], *ptr2 = polypool[poly2], 
			 *ptr3 = create_node(0, 0);
	polypool[num_polys++] = ptr3;
	
	while (ptr1 && ptr2) {
		if (ptr1->deg < ptr2->deg) {
			ptr3->deg = ptr1->deg;
			ptr3->coeff = ptr1->coeff;
			ptr1 = ptr1->next;
		}
		else if (ptr1->deg > ptr2->deg) {
			ptr3->deg = ptr2->deg;
			ptr3->coeff = ptr2->coeff;
			ptr2 = ptr2->next;
		}
		else {
			ptr3->deg = ptr1->deg;
			ptr3->coeff = ptr1->coeff + ptr2->coeff;
			ptr1 = ptr1->next;
			ptr2 = ptr2->next;
		}

		if (ptr1 || ptr2) {
			ptr3->next = create_node(0, 0);
			ptr3 = ptr3->next;
		}
	}
	while (ptr1) {
		ptr3->deg = ptr1->deg;
		ptr3->coeff = ptr1->coeff;
		ptr1 = ptr1->next;
		if (ptr1) {
			ptr3->next = create_node(0, 0);
			ptr3 = ptr3->next;
		}
	}
	while (ptr2) {
		ptr3->deg = ptr2->deg;
		ptr3->coeff = ptr2->coeff;
		ptr2 = ptr2->next;
		if (ptr2) {
			ptr3->next = create_node(0, 0);
			ptr3 = ptr3->next;
		}
	}
}

void mul() {

}

void disp(int poly_id) {
	polynode *ptr;
	
	printf("Poly Id %2d: ", poly_id);
	
	ptr = polypool[poly_id];
	printf("(%.2lf)x^%d", ptr->coeff, ptr->deg);
	
	ptr = ptr->next;	
	while (ptr) {
		printf(" + (%.2lf)x^%d", ptr->coeff, ptr->deg);
		ptr = ptr->next;
	}
	printf("\n");
}

void disp_all() {
	int i;
	for (i = 0; i < num_polys; i++) {
		disp(i);
	}
}

int main(void) {
	accept();
	accept(); 
	disp_all();
	add(0, 1);
	disp_all();
	return 0;
}

