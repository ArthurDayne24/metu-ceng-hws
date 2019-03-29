#ifndef TREE_H__
#define TREE_H__

#include <list>
#include <iostream>
#include <cmath>
#include <functional> //less<T> <-> operator<
//#include <queue>
//#include <stack>

using std::list;
using std::cout;
using std::endl;
using std::less;
//using std::queue;
//using std::stack;
                             //function object used for comparison, default value is less
template <typename Comparable, typename Comparator=less<Comparable> >
class Tree
{
public:
	Tree( );	//construct an empty tree
	Tree( const list<Comparable> & ); //construct complete BST with min height based on list vals
	~Tree( ); //release all dynamically allocated memory

	void construct( const list<Comparable> & ); //release own sources and get values from list

	const Comparable * findMin( ) const; //poInter to element that is minimal wrt Comparator, implemented
	const Comparable * findMax( ) const; //pointer to element that is maximal wrt Comparator, implemented

	const Comparable * getAddress( const Comparable & ) const; //find pointer to element with given values

	bool contains( const Comparable * ) const; //does this element with data exists in tree, implemented
	bool isEmpty( ) const; //is the tree empty, implemented

	void insert( const Comparable * ); //insert if dereferenced argument's data is not present, update heights of those affected
	void remove( const Comparable * ); //delete if dereferenced argument's data is present, update heights of those affected
	/* DO NOT FORGET TO CHECK BALANCE AND REBALANCE TREE IF NOT BALANCED AFTER INSERTION AND DELETION */

	size_t getSize( ) const; //accessor to size, implemented
	int getHeight( ) const; //accessor  to height, implemented
	bool isBalanced( ) const; //accessor to balanced, implemented

	void print( ) const; //inorder print of dereferenced pointers held in nodes, implemented
		
	friend class MusicList; //MusicList class can access private parts of Tree

private:
	struct TreeNode  //scope Tree::TreeNode, thus is only public to Tree structure, no non-friend outsiders can see
	{	 	  	  	 	   	     	    	 	
		const Comparable * element; //adress of the item
		TreeNode * left; //pointer to the root of left subtree
		TreeNode * right; //pointer to the root of right subtree
		TreeNode * parent; //pointer to parent node, pay attention to this!
		size_t height; //height >= 0

		//constructor
		TreeNode( const Comparable * el,
				  TreeNode * l, TreeNode * r, 
				  TreeNode * p )
		  : element( el ), 
			left( l ), right( r ),
			parent( p )	{}
		//you should separately set height values, no constructor is defined for them

	};

	TreeNode * root; //designated root

	/* isLessThan( const Comparable & lhs, const Comparable & rhs )
	* returns true if lhs<rhs and false otherwise */
	Comparator isLessThan;

	size_t size; //number of nodes
	bool balanced;

	/* uncopiable */
	Tree( const Tree & ); //disabled, provide no implementation
	Tree & operator=( const Tree & ); //disabled, provide no implementation

	/* private utility functions */
	TreeNode * findMin( TreeNode * ) const; //recursively find min in (leftward) subtrees
	TreeNode * findMax( TreeNode * ) const; //recursively find max in (rightward) subtrees
	bool contains( const Comparable *, TreeNode * ) const; //recursively search for an item

	bool isLeaf( TreeNode * ) const; //isLeaf
	bool isFull( TreeNode * ) const; //isFull
	void print( TreeNode * ) const; //recursive print

    //define and use more as you may need
    template <typename T>  //templated max
 	T max( T x, T y ) { return x > y ? x : y; }	 	  	  	 	   	     	    	 	

	void clearTree()
	{
		size=0;
		balanced=1;
		makeEmpty(root);
		root=NULL;
	}
	
	void recursiveConstructFromPtrs(typename list<const Comparable*>::const_iterator left,
                                    typename list<const Comparable*>::const_iterator right, 
                                    TreeNode*& node, TreeNode* parent)
	{
		//prepare mid
		typename list<const Comparable*>::const_iterator mid=chooseMid(left, right);

		//insert middle element
		node=new TreeNode(*mid, NULL, NULL, parent);

		typename list<const Comparable*>::const_iterator tmpl=mid;
		tmpl--;
		typename list<const Comparable*>::const_iterator tmpr=mid;
		tmpr++;
		
		//if there is at least one element to be inserted to the LEFT of node
		if(distance(left, mid)!=0)
			recursiveConstructFromPtrs(left, tmpl, node->left, node);

		//if there is at least one element to be inserted to the RIGHT of node
		if(distance(mid, right)!=0)
			recursiveConstructFromPtrs(tmpr, right, node->right, node);
		
		setHeight(node->left);	
		setHeight(node->right);	
		setHeight(node);	
	}

	void constructFromPtrs( list<const Comparable*> data )
	{
		clearTree();	
		size=data.size();
	
		if(!size)
			return;
		//else
		//	then there exists at least one element, continue		
	    recursiveConstructFromPtrs(data.begin(), --data.end(), root, NULL);
	}

	void setHeight(TreeNode* node)
	{
		if(!node) return;
	
		int l= (!node->left) ? -1 : node->left->height;
		int r= (!node->right) ? -1 : node->right->height;
		
		node->height=1+max(l,r);
	}

	void removeFromNode(const Comparable * data, TreeNode* & node)
	{	
		TreeNode* locater=getAddressFromNode(*data, node);
			
		if(!locater) return;

		TreeNode* parent=locater->parent;
		
		//Left and right exists
		if(locater->left && locater->right)
		{	
			locater->element=findMin(locater->right)->element;
			removeFromNode(locater->element, locater->right);
		}	 	  	  	 	   	     	    	 	

		//Leaf or only child
		else
		{
			TreeNode* tmp=locater;
			int flag;	

			if(parent)
				flag=(locater==parent->left) ? -1 : +1;
						
			locater=(locater->left) ? locater->left : locater->right;

			if(parent)
			{
				if(flag==+1)
					parent->right=locater;
				else//(flag==-1)
					parent->left=locater;
			}
			else
				root=locater;		

			if(locater)
				locater->parent=parent;

			tmp->left=tmp->right=tmp->parent=NULL;
			makeEmpty(tmp);

            //correct heights from node to root!					
			while(parent)
			{
				setHeight(parent);
				parent=parent->parent;
			}	 	  	  	 	   	     	    	 	
		}	
	}

	TreeNode* getAddressFromNode( const Comparable & item, TreeNode* node) const
	{	
		TreeNode* locater=node;
		
		while(locater)
		{
			if(isLessThan(item, *(locater->element)))
				locater=locater->left;
			else if(isLessThan(*(locater->element), item))
				locater=locater->right;
			else//if(*(locater->element)==item)
				break;
		}	 	  	  	 	   	     	    	 	
		
		return locater;
	}
	
	//It is not necessary to check if it is NULL
	void makeEmpty(TreeNode*& node)
	{
		if(!node) return;

		makeEmpty(node->left);
		makeEmpty(node->right);

		node->left=node->right=node->parent=NULL;
		node->element=NULL;		
		
		delete node;
		node=NULL;
	}

	void balanceTree()
	{	 	  	  	 	   	     	    	 	
		list<const Comparable*> List;
		toList(List, root);

		constructFromPtrs(List);
	}

	bool checkBalance()
	{
		if(isEmpty() || size==1 || root->height<2*(int)(log2(size)))
			return 1;
		return 0;	
	}
	
	void toList(list<const Comparable*> & List, TreeNode* node)
	{
		if(node)
		{
			toList(List, node->left);
			List.push_back(node->element);
			toList(List, node->right);	
		}
	}
	
	void printInOrder(TreeNode* node) const
	{
		if(!node) return;
		
		printInOrder(node->left);
		cout<<*(node->element)<<endl;
		printInOrder(node->right);
	}
	
	void printInOrder() const{printInOrder(root);}

	typename list<const Comparable*>::const_iterator chooseMid(
             typename list<const Comparable*>::const_iterator left,
             typename list<const Comparable*>::const_iterator right)
	{
		int N=distance(left, right)+1; 
	
		if(N==1)
			return left;//or right

		typename list<const Comparable*>::const_iterator mid=left;
		
		int x=log2(N);
		int adv=pow(2,x)-1;
		
		if(N-pow(2, x)<pow(2, x-1))
			adv=N-pow(2, x-1);
	
		advance(mid, adv);
	
		return mid;
	}

	list<const Comparable*> list2PtrList(const list<Comparable> & List)
	{
		list<const Comparable*> ptrList;
	
		typename list<Comparable>::const_iterator it=List.begin();
		typename list<Comparable>::const_iterator en=List.end();
	
		while(it!=en)
			ptrList.push_back(&*it++);

		return ptrList;	
	}

}; //Do not modify method prototypes and data member names

//Below this point you should begin implementing the missing parts

template <typename Comparable, typename Comparator>
Tree<Comparable,Comparator>::Tree( ) 
: root(NULL), size(0), balanced(1) {}

template <typename Comparable, typename Comparator>
Tree<Comparable,Comparator>::Tree( const list<Comparable> & data )
{
	//To indicate it is empty!
	root=NULL;
	construct(data);
}

template <typename Comparable, typename Comparator>
void Tree<Comparable,Comparator>::construct( const list<Comparable> & data )
{constructFromPtrs(list2PtrList(data));}	 	  	  	 	   	     	    	 	

template <typename Comparable, typename Comparator>
Tree<Comparable,Comparator>::~Tree(){clearTree();}

template <typename Comparable, typename Comparator>
void
Tree<Comparable,Comparator>::insert( const Comparable * data )
{
    //If the object already exists
	if(contains(data)) return;

    //If root is NULL, init. tree(root)
	
	if(!root)
	{
		root=new TreeNode(data, NULL, NULL, NULL);
		size=1;
		root->height=0;
		return;
	}
	
    //else{Find the correct position and insert there}
	
	TreeNode* locater=root;
	int flag;  //To know which side to add "data"
	
	while(true)
	{	 	  	  	 	   	     	    	 	
		if(isLessThan(*data, *(locater->element)))
		{
			if(locater->left) 
				locater=locater->left;
			else
			{
				flag=-1; 
				break;
			}
		}

		else// if(isLessThan(*(locater->element), *data))
		{
			if(locater->right) 
				locater=locater->right;
			else
			{
				flag=1; 
				break;
			}
		}
	}

    //insert new node to the appropriate position and pass to this node
	if(flag==-1)
	{
		locater->left=new TreeNode(data, NULL, NULL, locater);
        locater=locater->left;
    }
	else//(flag==+1)
	{
		locater->right=new TreeNode(data, NULL, NULL, locater);
		locater=locater->right;
	}

	//Set height values appropriately from node to the parent
	while(locater)
	{
		setHeight(locater);
		locater=locater->parent;
	}	 	  	  	 	   	     	    	 	

	size++;
	
	if(!checkBalance())
		balanceTree();
}

template <typename Comparable, typename Comparator>
void
Tree<Comparable,Comparator>::remove( const Comparable * data )
{
    //if there is no such an Comparable, do nothing!
	//the case in which root==NULL will already be handled here!
	if(!contains(data)) return;

	removeFromNode(data, root);
	size--;

	if(!checkBalance())
		balanceTree();
}

template <typename Comparable, typename Comparator>
const Comparable *
Tree<Comparable,Comparator>::findMin( ) const
{
  if( isEmpty( ) )
    return NULL;
  
  return findMin( root )->element; //call in to recursive utility function
}

template <typename Comparable, typename Comparator>
const Comparable *
Tree<Comparable,Comparator>::findMax( ) const
{
  if( isEmpty( ) )
    return NULL;
  
  return findMax( root )->element;
}

template <typename Comparable, typename Comparator>
const Comparable *
Tree<Comparable,Comparator>::getAddress( const Comparable & item ) const
{ 
	TreeNode* node=getAddressFromNode(item, root);
	if(node)
	    return node->element;
	else 
	    return NULL;
}

template <typename Comparable, typename Comparator>
bool
Tree<Comparable,Comparator>::contains( const Comparable * item ) const
{ 
  return contains( item, root );
}

template <typename Comparable, typename Comparator>
size_t
Tree<Comparable,Comparator>::getSize( ) const
{
  return size;
}

template <typename Comparable, typename Comparator>
int
Tree<Comparable,Comparator>::getHeight( ) const
{
  if( isEmpty( ) )
    return -1;  //convention: empty tree has height -1
  else
    return root->height; //>= 0
}	 	  	  	 	   	     	    	 	

template <typename Comparable, typename Comparator>
bool
Tree<Comparable,Comparator>::isEmpty( ) const
{
  return ( root == NULL );
}

template <typename Comparable, typename Comparator>
void
Tree<Comparable,Comparator>::print( ) const
{
  print( root );
  cout << endl;  
}

template <typename Comparable, typename Comparator>
bool
Tree<Comparable,Comparator>::isBalanced( ) const
{
  return balanced;
}

/* private utility functions 
 * define and use as many as you deem required
 * some are provided for the already implemented 
 * part of the public interface
 */
template <typename Comparable, typename Comparator>
typename Tree<Comparable,Comparator>::TreeNode *
Tree<Comparable,Comparator>::findMin( TreeNode * t ) const
{
  if( t == NULL )
    return NULL;

  if( t->left == NULL )
    return t;

  return findMin( t->left );
}	 	  	  	 	   	     	    	 	

template <typename Comparable, typename Comparator>
typename Tree<Comparable,Comparator>::TreeNode *
Tree<Comparable,Comparator>::findMax( TreeNode * t ) const
{
  if( t != NULL )
    while( t->right != NULL )
      t = t->right;
  return t;
}

template <typename Comparable, typename Comparator>
bool
Tree<Comparable,Comparator>::contains( const Comparable * item, 
                                           TreeNode * t ) const
{

  if( t == NULL )
    return false;
  else if( isLessThan( *item, *(t->element) ) )
    return contains( item, t->left );
  else if( isLessThan( *(t->element), *item ) )
    return contains( item, t->right );
  else
    return true;
}

template <typename Comparable, typename Comparator>
bool
Tree<Comparable,Comparator>::isLeaf( TreeNode * t ) const
{
  return ( ( t->left == NULL ) && ( t->right == NULL ) );
}

template <typename Comparable, typename Comparator>
bool
Tree<Comparable,Comparator>::isFull( TreeNode * t ) const
{	 	  	  	 	   	     	    	 	
  return ( ( t->left != NULL ) && ( t->right != NULL ) );
}

template <typename Comparable, typename Comparator>
void
Tree<Comparable,Comparator>::print( TreeNode * t ) const
{
  if( t && t->left != NULL )
  {
    cout << '[';
    print( t->left );
  }
  else if( t && t->left == NULL && t->right != NULL )
  {
    cout << "[";
  }

  if( t != NULL )
  {
    if( isLeaf( t ) )
    {
      cout << '(' << *( t->element ) << ')';
    }
    else if( !isLeaf( t ) )
    {
      cout << *( t->element );
    }
  }
  if( t && t->right != NULL )
  {
    print( t->right );
    cout << ']';
  }
  else if( t && t->left != NULL && t->right == NULL )
  {
    cout << "]";
  }	 	  	  	 	   	     	    	 	
}

#endif