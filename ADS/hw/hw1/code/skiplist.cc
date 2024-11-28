#include "skiplist.h"
#include <optional>

namespace skiplist {


skiplist_type::skiplist_type(double p):p(p),level(1) {
    head = new skiplist_node(0, "", max_level);
}

void skiplist_type::put(key_type key, const value_type &val) {
    int new_level = random_level();
    // if the new level is greater than the current level, update the head
    if(new_level>level){
        for(int i=level;i<new_level;i++){
            head->next[i]=nullptr;
        }
        level=new_level;
    }
    skiplist_node* cur=head;
    std::vector<skiplist_node*> update(level, nullptr);
    // find the position to insert at each level
    for(int i=level-1;i>=0;i--){
        while(cur->next[i]!=nullptr&&cur->next[i]->key<key){
            cur=cur->next[i];
        }
        update[i]=cur;
    }
    // if the key already exists, update the value
    if(cur->next[0]!=nullptr&&cur->next[0]->key==key){
        cur->next[0]->val=val;
        return;
    }
    // insert the new node of the new level
    skiplist_node* new_node=new skiplist_node(key, val, new_level);
    for(int i=0;i<new_level;i++){
        new_node->next[i]=update[i]->next[i];
        update[i]->next[i]=new_node;
    }
}

std::string skiplist_type::get(key_type key) const {
    skiplist_node* cur=head;
    //search the key from the top level to the bottom level,from the left to the right
    for(int i=level-1;i>=0;i--){
        while(cur->next[i]!=nullptr&&cur->next[i]->key<key){
            cur=cur->next[i];
        }
        // if the key is found, return the value
        if(cur->next[i]!=nullptr&&cur->next[i]->key==key){
            return cur->next[i]->val;
        }
    }
    return "";
}
//count the distance of the query which is the number of nodes visited during the search
int skiplist_type::query_distance(key_type key) const {
    skiplist_node* cur=head;
    int distance=1;
    //search the key from the top level to the bottom level,from the left to the right
    for(int i=level-1;i>=0;i--){
        while(cur->next[i]!=nullptr&&cur->next[i]->key<key){
            cur=cur->next[i];
            distance++;
        }
        // if the key is found, return the distance
        if(cur->next[i]!=nullptr&&cur->next[i]->key==key){
            distance++;
            return distance;
        }
        distance++;
    }
    return distance;
}


} // namespace skiplist
