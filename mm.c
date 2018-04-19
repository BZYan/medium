/*
 * mm.c -- implement your malloc functions here. See mm-naive.c for an example implementation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

typedef struct header hdr;
typedef struct footer ftr;


/* 
 * mm_init initializes the malloc package.
 */
struct header{
	size_t sns;
	hdr* next;
	hdr* prev;
	ftr* myfoot;
};

struct footer{
	size_t sns;
	hdr* myhead;
};

hdr* global_h;
void remov(hdr* h){
	h-> prev ->next = h -> next;
	h-> next ->prev = h -> prev;
}
int mm_init(void)
{
	global_h =mem_sbrk(align(sizeof(hdr)));
	global_h->sns= 32;
	global_h->next=global_h;
	global_h->prev=global_h;
	global_h->myfoot=NULL;
	return 0;
}

void* find_fit(size_t size){
	hdr* cursor;
	for(cursor= global_h->next; cursor != global_h && cursor->sns < size; cursor= cursor-> next){
//mm_checkheap(1);
//printf("loc at %p \n",cursor);
	
//printf("requested size is %d cursor size is %d\n",(int)size, (int)cursor->sns);
	}
	if (cursor!= global_h) {
//printf("hit\n");
		return cursor;
	}
	else {
//printf("miss\n");
		return NULL;	
	}
}
void split(hdr* h, size_t asize, int version){
			size_t ori_s=h->sns;
			hdr* new_h=(hdr*)((char*)h+asize);
			ftr* f= h->myfoot;
			ftr* new_f=(ftr*)((char*)new_h-16);
			
			h->sns=asize+1;
			h->myfoot=new_f;
			new_f->sns=h->sns;
			new_f->myhead=h;
			
			f->myhead=new_h;
			new_h->myfoot=f;
			new_h->sns=ori_s-asize;
			f->sns=new_h->sns;
			
			if(version!=42){
			new_h->next=h->next;
			new_h->prev=h;
			h->next->prev=new_h;
			h->next=new_h;
			remov(h);
			}
			

}
/* 
 * mm_malloc allocates a memory block of size bytes
 */
void *mm_malloc(size_t size)
{
	if (size<=0) return NULL;
	
	size_t asize=align(size+align(sizeof(hdr))+align(sizeof(ftr)));
	hdr* h= find_fit(asize);
	ftr * f;
	if(h==NULL){
		h= mem_sbrk(asize);
		if((long) h == -1) return NULL;

		else{ 		//if memory is enough
			h->sns= asize+1;
			h->prev=NULL;
			h->next=NULL;
			f=(ftr *) ((char* )h+ asize - align(sizeof(ftr)));
			h->myfoot=f;
			f->myhead=h;
			f->sns= h->sns;
		}
	}else{		//if found in find_fit
		if(h->sns <=(asize+48)){
			h-> sns ++;
			remov(h);
			f =h->myfoot;
			f-> sns = h->sns ;
		} else{
			split(h,asize,0);
			
		}
	}
	return (char *) h + align(sizeof(hdr));
}

/*
 * mm_free frees the previously allocated memory block
 */
int neighbor(hdr* h){		//return 0 for none, 1 right, 2 left, 3 both
	ftr* f = h-> myfoot;
	int rightflag=0;
	int leftflag=0;
	if(((char*)f+sizeof(ftr)-1)!= mem_heap_hi()){
		hdr* r_h = (hdr*) ((char*)f + 16);
		if( !(r_h -> sns &1) ){rightflag++;} 
	}
	if( ( (char*)h -sizeof(hdr) ) != mem_heap_lo()){
		ftr* l_f =(ftr*) ((char*)h - 16);
		if( !(l_f -> sns &1) ){leftflag++;}
	}  
	if (leftflag==0 && rightflag==0 ) return 0;
	else if(rightflag==1 && leftflag==0) return 1;
	else if(rightflag==0 && leftflag==1) return 2;
	else return 3;
}


void mm_free(void *ptr)
{
//printf("trying to free %p\n",ptr);
	hdr* this_h=(hdr*)((char*)ptr-32);
	this_h->sns --;
	ftr* this_f= this_h -> myfoot;
	this_f->sns=this_h->sns;
	int flag=neighbor(this_h);
//printf("flag is %d\n",flag);
	if(flag==1 || flag ==3){
//printf("right clse\n");
		hdr* r_h = (hdr*) ((char*)this_f + sizeof(ftr));
		remov(r_h);
		ftr* r_f = r_h -> myfoot;
		this_h -> sns += r_f -> sns;
		r_f -> sns = this_h -> sns;
		this_h -> myfoot = r_f;
		r_f -> myhead = this_h;
		this_f = r_f;
	}else if(flag==2 || flag==3){
//printf("left clse\n");
		ftr* l_f =(ftr*) ((char*)this_h - sizeof(ftr));
		hdr* l_h = l_f -> myhead;
		this_f -> sns += l_h -> sns;
		l_h -> sns = this_f -> sns;
		this_f -> myhead = l_h;
		l_h -> myfoot = this_f;
		this_h = l_h;
		remov(l_h);
//mm_checkheap(1);
	}
	
	this_h->next = global_h->next;
	global_h->next->prev = this_h;
	this_h->prev = global_h;
	global_h->next = this_h;
//printf("free complete \n");
}	

/*
 * mm_realloc changes the size of the memory block pointed to by ptr to size bytes.  
 * The contents will be unchanged in the range from the start of the region up to the minimum of   
 * the  old  and  new sizes.  If the new size is larger than the old size, the added memory will   
 * not be initialized.  If ptr is NULL, then the call is equivalent  to  malloc(size).
 * if size is equal to zero, and ptr is not NULL, then the call is equivalent to free(ptr).
 */
void *mm_realloc(void *ptr, size_t size)
{
	// Your code here
//printf("realloc called\n");
	if(ptr==NULL){
		return mm_malloc(size);
	}else if(size==0){
		mm_free(ptr);
	}else{
		size_t nsize=align(size);
		hdr* h=(hdr*)((char*)ptr-32);
		if(nsize< (h->sns-1-48)){
			split(h,nsize+48,42);
			return (void*)((char*)h+32);
		}else{
			if(neighbor(h)==1 || neighbor(h)==3){
				
			}else{
				void* ret= mm_malloc(nsize);
				memcpy(ret,ptr,h->sns-49);
				mm_free(ptr);
				return ret;
			}
		}
	}
	return NULL;
}


/*
 * mm_checkheap checks the integrity of the heap and helps with debugging
 */
void mm_checkheap(int verbose_level) 
{
	printf("head lo is %p\n",mem_heap_lo());
	printf("the current size is %d\n",(int)mem_heapsize());
	printf("global header is at %p\n",global_h);
	hdr* cursor=mem_heap_lo();
	while(cursor < (hdr*)mem_heap_hi() ){
		if(verbose_level==0){
			printf("%s block at %p, size %d\n", (cursor->sns&1)?"allocated":"free",
			cursor,(int)cursor->sns & ~1);
			if(cursor!=global_h) printf("ftr is at %p, sns %d\n", cursor->myfoot, (int)cursor->myfoot->sns);
		}
		if(verbose_level==1 && (cursor->sns %2)==0){
			printf("free block at %p, sns %d\n",cursor, (int)cursor->sns);
			if(cursor!=global_h) printf("ftr is at %p, sns %d\n", cursor->myfoot, (int)cursor->myfoot->sns);
		}
			cursor= (hdr*)( (char*)cursor+ (cursor->sns & ~1) ); 
	}
	if(verbose_level!=-3){
		printf("global header is at %p\n",global_h);
		cursor=global_h->next;
		int count=1;
		while(cursor!=global_h){
			printf("the %d item at %p, sns is %d, ftr is at %p\n",count,cursor,(int)cursor->sns,cursor->myfoot);
			count++;
			cursor=cursor->next;
		}
	}	
}


