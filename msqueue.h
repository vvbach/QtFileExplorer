#ifndef MSQUEUE_H
#define MSQUEUE_H

#include <atomic>

template <typename T>
class MSQueue {
private:
    struct Node {
        T value;
        std::atomic<Node*> next;

        Node(const T& val) : value(val), next(nullptr) {}
        Node() : next(nullptr) {} // dummy node
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    MSQueue() {
        Node* dummy = new Node();  // dummy node
        head.store(dummy);
        tail.store(dummy);
    }

    ~MSQueue() {
        clear();
        delete head.load();
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        while (true) {
            Node* last = tail.load();
            Node* next = last->next.load();

            if (next == nullptr) {
                if (last->next.compare_exchange_weak(next, newNode)) {
                    tail.compare_exchange_weak(last, newNode);
                    return;
                }
            } else {
                tail.compare_exchange_weak(last, next);
            }
        }
    }

    bool dequeue(T& out) {
        while (true) {
            Node* first = head.load();
            Node* next = first->next.load();

            if (next == nullptr) {
                return false;
            }

            if (head.compare_exchange_weak(first, next)) {
                out = next->value;
                delete first;
                return true;
            }
        }
    }

    void clear(){
        T temp;
        while (dequeue(temp));
    }
};

#endif // MSQUEUE_H
