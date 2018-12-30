

/////////////////////////////////////////////////////////////////////////////
////Класс описывающий простой список
/////////////////////////////////////////////////////////////////////////////
class ListNode
{
  protected:
             ListNode*  prev;
             ListNode*  last;
             void*      data;
  public:
             ListNode(void* d=NULL,ListNode* nx,ListNode* pr)
                            :data(d), next(nx), prev(pr) {}
             void*          Data() { return data; }
             ListNode*      Next() { return next; }
             ListNode*      Prev() { return prev; }
     static  void           Delete(ListNode* el);
     static  void           Insert(ListNode* el,ListNode* prev=NULL,ListNode* next=NULL);
};

class List
{
  private:
    ListNode    *first,
                *last;

  public:
          ListNode(int d,ListNode* nxt = NULL): next(nxt), data(d) {}
          ~ListNode() { delete next; }
          int         Data()  { return data; }
          ListNode*   Next()  { return next; }
};


void    ListNode::Insert(ListNode* el,ListNode* prev,ListNode* next)
{
   el->prev = prev;
   el->next = next;
   if (prev)
      prev->next = el;
   else
      prev = el;
   if (next)
      next->prev = el;
   else
      next = el;
}

void    ListNode::Delete(ListNode* el)
{
   el->prev->next = el->next;
}