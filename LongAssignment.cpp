#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <sstream>
using namespace std;

template <typename T>
static void reverse(vector<T> &p)
{
    int i = 0;
    int j = (int)p.size() - 1;
    while (i < j)
    {
        T temp = p[i];
        p[i] = p[j];
        p[j] = temp;
        i++;
        j--;
    }
}

// TreeNode represents a version of a file .
struct TreeNode
{
    int version_id;
    string content;
    string message; // empty if not a snapshot.
    TreeNode *parent;
    vector<TreeNode *> children; // multiple branches possible
    time_t created_timestamp;
    time_t snapshot_timestamp; // null if not a snapshot
    TreeNode(int id, string data, string msg = "")
    {
        version_id = id;
        content = data;
        parent = nullptr;
        message = msg;
        created_timestamp = time(nullptr);
        snapshot_timestamp = 0;
    }
};

// ---------------- HASHMAP ----------------
struct HashNode
{
    int key;
    TreeNode *value;
    HashNode *next;

    HashNode(int k, TreeNode *v)
    {
        key = k;
        value = v;
        next = nullptr;
    }
};

class HashMap
{
private:
    int capacity;
    vector<HashNode *> Map_table;

    int hashFunction(int key)
    {
        return key % capacity;
    }

public:
    HashMap(int size = 100)
    {
        capacity = size;
        Map_table.resize(capacity, nullptr);
    }
    void insert(int key, TreeNode *value)
    {
        int idx = hashFunction(key);
        HashNode *newNode = new HashNode(key, value);

        if (Map_table[idx] == nullptr)
        {
            Map_table[idx] = newNode;
        }
        else
        {
            HashNode *temp = Map_table[idx];
            while (temp->next != nullptr && temp->key != key)
            {
                temp = temp->next;
            }
            if (temp->key == key)
            {
                temp->value = value;
                delete newNode;
            }
            else
            {
                temp->next = newNode;
            }
        }
    }
    TreeNode *Search(int key)
    {
        int idx = hashFunction(key);
        HashNode *temp = Map_table[idx];
        while (temp)
        {
            if (temp->key == key)
                return temp->value;
            temp = temp->next;
        }
        return nullptr;
    }
    void Delete(int key)
    {
        int index = hashFunction(key);
        HashNode *temp = Map_table[index];
        HashNode *last = nullptr;

        while (temp != nullptr && temp->key != key)
        {
            last = temp;
            temp = temp->next;
        }

        if (temp == nullptr)
            return; // not found

        if (last == nullptr)
        {
            Map_table[index] = temp->next;
        }
        else
        {
            last->next = temp->next;
        }

        delete temp;
    }
};

// File : Each object of file class is a tree with versions as nodes .
class File
{
private:
    TreeNode *root;
    TreeNode *active_version;
    int total_versions;
    HashMap versionMap;

public:
    File()
    {
        total_versions = 1;
        root = new TreeNode(total_versions - 1, "", "Initial Version");
        active_version = root;
        versionMap.insert(root->version_id, root);
        active_version->snapshot_timestamp = time(nullptr);
    };

    ~File()
    {
        if (!root)
            return;
        vector<TreeNode *> stack;
        stack.push_back(root);
        while (!stack.empty())
        {
            TreeNode *node = stack.back();
            stack.pop_back();
            for (TreeNode *child : node->children)
                stack.push_back(child);
            versionMap.Delete(node->version_id);
            delete node;
        }
        root = nullptr;
        active_version = nullptr;
    }

    void Read()
    {
        if (active_version)
            cout << active_version->content;
    }

    void Insert(string newContent)
    {
        if (!active_version)
            return;
        if (active_version->snapshot_timestamp != 0)
        {
            total_versions++;
            TreeNode *newNode = new TreeNode(total_versions - 1, newContent);
            newNode->parent = active_version;
            active_version->children.push_back(newNode);
            active_version = newNode;
            versionMap.insert(newNode->version_id, newNode);
            active_version->created_timestamp = time(nullptr);
        }
        else
            active_version->content += newContent;
    }

    void Update(string newContent)
    {
        if (!active_version)
            return;
        if (active_version->snapshot_timestamp != 0)
        {
            total_versions++;
            TreeNode *newNode = new TreeNode(total_versions - 1, newContent);
            newNode->parent = active_version;
            active_version->children.push_back(newNode);
            active_version = newNode;
            versionMap.insert(newNode->version_id, newNode);
            active_version->created_timestamp = time(nullptr);
        }
        else
            active_version->content = newContent;
    }

    void Snapshot(const string &snapshot_msg)
    {
        if (active_version->snapshot_timestamp == 0)
        {
            active_version->message = snapshot_msg;
            active_version->snapshot_timestamp = time(nullptr);
        }
    }

    void Rollback(int Version_id = -1)
    {
        if (!active_version)
            return;
        if (Version_id == -1)
        {
            if (active_version->parent)
            {   
                cout << "Rolled back from version " << active_version->version_id ;
                active_version = active_version->parent;
                cout << " to version " << active_version->version_id << endl;
            }
            else
                cout << "No parent version to roll back to!" << endl;
        }
        else
        {
            TreeNode *node = versionMap.Search(Version_id);
            if (node)
            {
                active_version = node;
                cout << "Rolled back to version " << Version_id << endl;
            }
            else
                cout << "Version " << Version_id << " not found!" << endl;
        }
    }

    void History()
    {
        cout << "--------------- HISTORY -----------------" << endl;
        vector<TreeNode *> snapshots;
        TreeNode *curr = active_version;
        while (curr != nullptr)
        {
            if (curr->snapshot_timestamp != 0)
                snapshots.push_back(curr);
            curr = curr->parent;
        }
        //reverse(snapshots);   //
        for (TreeNode *node : snapshots)
        {
            string tstr = ctime(&node->snapshot_timestamp);
            if (!tstr.empty() && tstr.back() == '\n')
                tstr.pop_back();
            cout << "Version ID : " << node->version_id
                 << ", Snapshot Time: " << tstr
                 << ", Message: " << node->message << endl;
        }
        cout << "------------------------------------------" << endl;
    }

    TreeNode *getActiveVersion() { return active_version; }
};

struct HeapNode
{
    string file_name;
    File *filePtr;
    long long update_counter; // replaces timestamp
    int total_versions;
    int index;

    HeapNode(const string &fname, File *fptr, long long counter, int idx, int versions = 1)
        : file_name(fname), filePtr(fptr), update_counter(counter), total_versions(versions), index(idx) {}
};

struct MapNode
{
    string key;
    HeapNode *heapPtr;
    MapNode *next;
    MapNode(const string &k, HeapNode *ptr) : key(k), heapPtr(ptr), next(nullptr) {}
};

class CustomMap
{
private:
    int capacity;
    vector<MapNode *> table;
    int hashFunction(const string &key)
    {
        int sum = 0;
        for (char c : key)
            sum += c;
        return sum % capacity;
    }

public:
    CustomMap(int size = 100)
    {
        capacity = size;
        table.resize(capacity, nullptr);
    }
    void insert(const string &key, HeapNode *heapPtr)
    {
        int idx = hashFunction(key);
        MapNode *newNode = new MapNode(key, heapPtr);
        if (!table[idx])
            table[idx] = newNode;
        else
        {
            MapNode *temp = table[idx];
            while (temp->next && temp->key != key)
                temp = temp->next;
            if (temp->key == key)
            {
                temp->heapPtr = heapPtr;
                delete newNode;
            }
            else
                temp->next = newNode;
        }
    }
    HeapNode *get(const string &key)
    {
        int idx = hashFunction(key);
        MapNode *temp = table[idx];
        while (temp)
        {
            if (temp->key == key)
                return temp->heapPtr;
            temp = temp->next;
        }
        return nullptr;
    }
    void remove(const string &key)
    {
        int idx = hashFunction(key);
        MapNode *temp = table[idx];
        MapNode *last = nullptr;
        while (temp && temp->key != key)
        {
            last = temp;
            temp = temp->next;
        }
        if (!temp)
            return;
        if (!last)
            table[idx] = temp->next;
        else
            last->next = temp->next;
        delete temp;
    }
};

// Max Heap : Nodes represent individual files .
class MaxHeap
{
private:
    vector<HeapNode *> heap;
    CustomMap map;
    long long global_counter = 0; // increments with each insertOrUpdate

    void swapNodes(int i, int j)
    {
        swap(heap[i], heap[j]);
        heap[i]->index = i;
        heap[j]->index = j;
    }

    void heapifyUp(int idx)
    {
        while (idx > 0)
        {
            int parent = (idx - 1) / 2;
            if (heap[parent]->update_counter >= heap[idx]->update_counter)
                break;

            swapNodes(parent, idx);
            idx = parent;
        }
    }

    void heapifyDown(int idx)
    {
        int n = heap.size();
        while (true)
        {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int largest = idx;

            if (left < n && heap[left]->update_counter > heap[largest]->update_counter)
                largest = left;

            if (right < n && heap[right]->update_counter > heap[largest]->update_counter)
                largest = right;

            if (largest == idx)
                break;

            swapNodes(idx, largest);
            idx = largest;
        }
    }

public:
    MaxHeap() : map(200), global_counter(0) {}

    void insertOrUpdate(const string &file_name)
    {
        HeapNode *node = map.get(file_name);
        global_counter++;
        if (node)
        {
            node->update_counter = global_counter;
            node->total_versions++;
            heapifyUp(node->index);
            heapifyDown(node->index);
        }
        else
        {
            int idx = heap.size();
            HeapNode *newNode = new HeapNode(file_name, nullptr, global_counter, idx, 1);
            heap.push_back(newNode);
            map.insert(file_name, newNode);
            heapifyUp(idx);
        }
    }

    void insertOrUpdate(const string &file_name, File *fptr)
    {
        HeapNode *node = map.get(file_name);
        global_counter++;
        if (node)
        {
            node->update_counter = global_counter;
            node->total_versions++;
            node->filePtr = fptr;
            heapifyUp(node->index);
            heapifyDown(node->index);
        }
        else
        {
            int idx = heap.size();
            HeapNode *newNode = new HeapNode(file_name, fptr, global_counter, idx, 1);
            heap.push_back(newNode);
            map.insert(file_name, newNode);
            heapifyUp(idx);
        }
    }

    HeapNode *getMax() { return heap.empty() ? nullptr : heap[0]; }

    void printHeap_recent(int num)
    {
        cout << " RECENT FILES (most recent first):\n";
        vector<HeapNode *> tempHeap = heap;

        auto heapifyDownTemp = [](vector<HeapNode *> &h, int idx)
        {
            int n = h.size();
            while (true)
            {
                int left = 2 * idx + 1;
                int right = 2 * idx + 2;
                int largest = idx;

                if (left < n && h[left]->update_counter > h[largest]->update_counter)
                    largest = left;
                if (right < n && h[right]->update_counter > h[largest]->update_counter)
                    largest = right;
                if (largest == idx)
                    break;
                swap(h[idx], h[largest]);
                idx = largest;
            }
        };

        int count = 0;
        while (!tempHeap.empty() && count < num)
        {
            HeapNode *top = tempHeap[0];
            cout << top->file_name << endl;
            tempHeap[0] = tempHeap.back();
            tempHeap.pop_back();
            if (!tempHeap.empty())
                heapifyDownTemp(tempHeap, 0);
            count++;
        }
    }

    void printHeap_biggest(int num)
    {

        cout << " BIGGEST TREES (most versions first):\n";
        vector<HeapNode *> tempHeap = heap;

        auto heapifyDownTemp = [](vector<HeapNode *> &h, int idx)
        {
            int n = h.size();
            while (true)
            {
                int left = 2 * idx + 1;
                int right = 2 * idx + 2;
                int largest = idx;
                if (left < n && h[left]->total_versions > h[largest]->total_versions)
                    largest = left;
                if (right < n && h[right]->total_versions > h[largest]->total_versions)
                    largest = right;
                if (largest == idx)
                    break;
                swap(h[idx], h[largest]);
                idx=largest;
            }
        };

        // Build a max heap by total_versions
        for (int i = (int)tempHeap.size() / 2 - 1; i >= 0; i--)
        {
            heapifyDownTemp(tempHeap, i);
        }

        int count = 0;

        while (!tempHeap.empty() && count < num)
        {
            HeapNode *top = tempHeap[0];
            cout << top->file_name << " : " << top->total_versions << " versions\n";
            tempHeap[0] = tempHeap.back();
            tempHeap.pop_back();
            if (!tempHeap.empty())
                heapifyDownTemp(tempHeap, 0);
            count++;
        }
    }
};

class FileSystem
{

private:
    MaxHeap fileHeap;
    CustomMap fileMap;

public:
    FileSystem() : fileMap(200) {}

    void create(const string &filename)
    {
        if (fileMap.get(filename))
        {
            cout << "File already exists: " << filename << endl;
            return;
        }
        File *newFile = new File();
        fileHeap.insertOrUpdate(filename, newFile);
        fileMap.insert(filename, fileHeap.getMax());
        cout << "File created: " << filename << endl;
    }

    void read(const string &filename)
    {
        HeapNode *node = fileMap.get(filename);
        if (!node)
        {
            cout << "File not found: " << filename << endl;
            return;
        }
        node->filePtr->Read();
        cout << endl;
    }

    void insert(const string &filename, const string &content)
    {
        HeapNode *node = fileMap.get(filename);
        if (!node)
        {
            cout << "File not found: " << filename << endl;
            return;
        }
        node->filePtr->Insert(content);
        fileHeap.insertOrUpdate(filename, node->filePtr);
    }

    void update(const string &filename, const string &content)
    {
        HeapNode *node = fileMap.get(filename);
        if (!node)
        {
            cout << "File not found: " << filename << endl;
            return;
        }
        node->filePtr->Update(content);
        fileHeap.insertOrUpdate(filename, node->filePtr);
    }

    void snapshot(const string &filename, const string &message)
    {
        HeapNode *node = fileMap.get(filename);
        if (!node)
        {
            cout << "File not found: " << filename << endl;
            return;
        }
        node->filePtr->Snapshot(message);
        // fileHeap.insertOrUpdate(filename, node->filePtr);  //Not being counted as modification.
    }

    void rollback(const string &filename, int versionID = -1)
    {
        HeapNode *node = fileMap.get(filename);
        if (!node)
        {
            cout << "File not found: " << filename << endl;
            return;
        }
        node->filePtr->Rollback(versionID);
       // fileHeap.insertOrUpdate(filename, node->filePtr);  //Not being counted as modification.
    }

    void history(const string &filename)
    {
        HeapNode *node = fileMap.get(filename);
        if (!node)
        {
            cout << "File not found: " << filename << endl;
            return;
        }
        node->filePtr->History();
    }
    void printRecentFiles(int n) { fileHeap.printHeap_recent(n); }

    void printBiggestFiles(int n) { fileHeap.printHeap_biggest(n); }
};

bool is_valid_Command(const string &s)
{
    static vector<string> cmds = {"CREATE", "READ", "INSERT", "UPDATE", "SNAPSHOT", "ROLLBACK", "HISTORY", "BIGGEST_TREES", "RECENT_FILES"};
    for (size_t i = 0; i < cmds.size(); i++)
        if (cmds[i] == s)
            return true;
    return false;
}

int main()
{
    FileSystem fs;
    string line;
    while (getline(cin, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        string token;
        iss >> token;

        if (token == "CREATE")
        {
            string fname;
            iss >> fname;
            fs.create(fname);
        }

        else if (token == "READ")
        {
            string fname;
            iss >> fname;
            fs.read(fname);
        }

        else if (token == "INSERT" || token == "UPDATE" || token == "SNAPSHOT")
        {
            string fname;
            iss >> fname;
            string msg, word;
            while (iss >> word)
            {
                if (is_valid_Command(word))
                    break;
                if (!msg.empty())
                    msg += " ";
                msg += word;
            }
            if (token == "INSERT")
                fs.insert(fname, msg);
            else if (token == "UPDATE")
                fs.update(fname, msg);
            else
                fs.snapshot(fname, msg);
        }

        else if (token == "ROLLBACK")
        {
            string fname;
            iss >> fname;
            string maybe;
            if (iss >> maybe && !is_valid_Command(maybe))
                fs.rollback(fname, stoi(maybe));
            else
                fs.rollback(fname);
        }

        else if (token == "HISTORY")
        {
            string fname;
            iss >> fname;
            fs.history(fname);
        }

        else if (token == "BIGGEST_TREES")
        {
            int n;
            iss >> n;
            cout<<n;
            fs.printBiggestFiles(n);
        }

        else if (token == "RECENT_FILES")
        {
            int n;
            iss >> n;
            cout<<n;
            fs.printRecentFiles(n);
        }

        else
            cout << "Unknown command: " << token << endl;
    }
    return 0;
}
