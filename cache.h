//
//  cache.h
//  grub-fuse
//
//  Created by Albert Zeyer on 20.09.11.
//  Copyright 2011 Albert Zeyer. All rights reserved.
//

#ifndef grub_fuse_cache_h
#define grub_fuse_cache_h

#include <list>
#include <map>

template<typename KeyType, typename ValueType, size_t CacheSize>
struct Cache {
	struct Item;
	typedef std::map<KeyType,Item> KVmap;
	struct Item {
		Item *left, *right;
		KeyType key;
		ValueType value;
		Item() : left(NULL), right(NULL) {}
	};
	
	KVmap kvmap;
	Item round_link_item; // left = bottom; right = top

	void _item_take_out(Item* item) {
		item->left->right = item->right;
		item->right->left = item->left;
		item->left = NULL;
		item->right = NULL;
	}

	void _check_list_item_count(Item* item) {
		bool was_in_list = item->left || item->right;
		if(was_in_list) return;
		if(kvmap.size() <= CacheSize) return;
		Item* bottom = round_link_item.left;
		_item_take_out(bottom);

	}

	void _push_up_item(Item* item) {
		_check_list_item_count(item);
		_item_take_out(item);
		item->left = &round_link_item;
		item->right = round_link_item.right;
		round_link_item.right = item;
		top = item;
	}

	Item* _return_item(Item& item) {
		_push_up_item(&item);
		return &item;
	}

	Item* lower_bound(const KeyType& key) {
		KVmap::iterator i = kvmap.lower_bound(key);
		if(i != kvmap.end() && i->first == key)
			return _return_item(i->second);
		// i->first > key here.

		if(i == kvmap.begin())
			// we cannot step back. there is no lower_bound < key.
			return NULL;

		--i;
		// i->first < key here. and it is the lower_bound
		return _return_item(i->second);
	}

	void push_back(const KeyType& key, const ValueType& value) {
		Item& item = kvmap[key];
		item.key = key;
		item.value = value;
		_push_up_item(&item);
	}
};

#endif
