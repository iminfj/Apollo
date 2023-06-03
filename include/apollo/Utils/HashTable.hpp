/*
 * This file is part of the [Apollo-LS] project.
 * Copyright [2023] [Rui Zhang] and other contributors.
 * See the LICENSE file in the project root directory for the full license information.
 * 
 * The sharing rights for this project belong to all those who have contributed to the project.
 * 
 * Thanks to the following contributors for their dedication and support of this project:
 *   - Rui Zhang 
*/


# include <iostream>

using namespace std;


/* HashTable - string key-value pairs implementation */
class HashTable {
    public:
        HashTable() {
            // cout << "Hello HahsTable is Run!" << endl;
        }
        ~HashTable() {
            // cout << "Bye~ HashTable is Release!" << endl;
        }
        bool 	put			(string, string);
        bool 	put			(string, string, string);
        bool 	update		(string, string);
        string 	get			(string);
        string	get_nohint	(string);
        string	get_desc	(string);

		void	keys_write	(const string &);
		void	vals_write	(const string &);
		void	desc_write	(const string &);

        struct keys {
            string key = "";
            struct keys * next;
        };
        struct keys * keys_t;

        struct vals {
            string key = "";
            string value = "";
            struct vals * next;
        };
        struct vals * vals_t;

        struct desc {
            string key = "";
            string descript = "";
            struct desc * next;
        };
        struct desc * desc_t;
/* 
		//	   β(a)	β(b)
		vector<int, string> beta; */
};
HashTable expr_var;


/* Define */

void HashTable::keys_write (const string &__loc) 
{
	FILE * w = fopen(__loc.c_str(), "w");
	string data("");
	for (struct keys * k = keys_t; k; k = k->next) 
	{
		if (!k->key.empty()) {
			data.append("[");
			data.append(k->key);
			data.append("]");
		}
	}
	if (!data.empty()) 
		fprintf(w, "%s", data.c_str());
	fclose(w);
}
void HashTable::vals_write (const string &__loc) 
{
	FILE * w = fopen(__loc.c_str(), "w");
	string data("");
	for (struct vals * v = vals_t; v; v = v->next) 
	{
		if (!v->value.empty()) {
			data.append("[");
			data.append(v->value);
			data.append("]");
		}
	}
	if (!data.empty()) 
		fprintf(w, "%s", data.c_str());
	fclose(w);
}
void HashTable::desc_write (const string &__loc) 
{
	FILE * w = fopen(__loc.c_str(), "w");
	string data("");
	for (struct desc * d = desc_t; d; d = d->next) 
	{
		if (!d->descript.empty()) {
			data.append("[");
			data.append(d->descript);
			data.append("]");
		}
	}
	if (!data.empty()) 
		fprintf(w, "%s", data.c_str());
	fclose(w);
}


string HashTable::get_desc (string key) {
	for (struct keys * k = keys_t; k; k = k->next) {
		if (k->key == key) {
			for (struct desc * d = desc_t; d; d = d->next) {
				if (d->key == k->key) return d->descript;
			}
		}
	}
	return "";
}

bool HashTable::put (string key, string value) {
	/* The presence of k directly overrides v->value.s */
	if (get_nohint(key) != "") {
		struct vals * v = new struct vals;
		/* Here, because I have enough confidence in my logic, I just 
		   skip the judgment. 
		*/
		v->value = value;
		return true;
	}

	struct keys * k = new struct keys;
	struct vals * v = new struct vals;

	k->key		= key;
	v->key 		= key;
	v->value	= value;

	k->next		= keys_t;
	keys_t		= k;
	v->next		= vals_t;
	vals_t		= v;

	return true;
}

bool HashTable::put (string key, string value, string desc) {
	/* The presence of k directly overrides v->value.s */
	if (get_nohint(key) != "") {
		struct vals * v = new struct vals;
		/* Here, because I have enough confidence in my logic, I just 
		   skip the judgment. 
		*/
		v->value = value;
		return true;
	}

	struct keys * k = new struct keys;
	struct vals * v = new struct vals;
	struct desc * d = new struct desc;

	k->key		= key;
	v->key 		= key;
	v->value	= value;
	d->key		= key;
	d->descript	= desc;

	k->next		= keys_t;
	keys_t		= k;
	v->next		= vals_t;
	vals_t		= v;
	d->next		= desc_t;
	desc_t		= d;

	return true;
}

string HashTable::get (string key) {
	for (struct keys * k = keys_t; k; k = k->next) {
		if (k->key == key) {
			for (struct vals * v = vals_t; v; v = v->next) {
				if (v->key == k->key) return v->value;
			}
			cout << "\n\tSorry, no value bound to key(" << k->key << ") found." << endl;
			return "";
		}
	}
	cout << "\n\tSorry, please check if you have put this key(" << key << ") to the HashTable." << endl;
	return "";
}

string HashTable::get_nohint (string key) {
	for (struct keys * k = keys_t; k; k = k->next) {
		if (k->key == key) {
			for (struct vals * v = vals_t; v; v = v->next) {
				if (v->key == k->key) return v->value;
			}
		}
	}
	return "";
}