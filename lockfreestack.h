

#include <atomic>
#include <memory>

template <typename T>

class LockfreeStack {
    public:
        struct Node {
            std::shared_ptr<T> m_data;
            Node* m_next;

            Node(const T& value):
                    m_data { std::make_shared<T>(value) },
                    m_next { nullptr }
            { }
        };


        LockfreeStack():
                m_head { nullptr },
                m_deleted_nodes { nullptr },
                m_threads_in_pop { 0 }
        { }


        void push(const T& new_value)
        {
            auto new_head = new Node { new_value };
            new_head->m_next = m_head.load();
            while (!m_head.compare_exchange_weak(new_head->m_next, new_head));
        }


        bool pop()
        {
            m_threads_in_pop.fetch_add(1);

            Node* old_head = m_head.load();
            while (old_head && !m_head.compare_exchange_weak(old_head, old_head->m_next));

            bool res_data = old_head ? true : false;
            try_reclaim(old_head);

            return res_data;
        }


        std::shared_ptr<T> top()
        {
            Node* old_head = m_head.load();
            auto res_data = old_head ? old_head->m_data : nullptr;

            return res_data;
        }




    private:
        void try_reclaim(Node* old_head)
        {
            if (m_threads_in_pop == 1) {

                auto nodes = m_deleted_nodes.exchange(nullptr);
                if (!--m_threads_in_pop) {
                    delete_nodes(nodes);
                }
                else if (nodes) {

                }

                delete old_head;
            }
        }


        void delete_nodes(Node* list)
        {
            while (list) {
                Node* next = list->m_next;
                delete list;
                list = next;
            }
        }

    private:
        std::atomic<Node*> m_head;
        std::atomic<Node*> m_deleted_nodes;
        std::atomic<int> m_threads_in_pop;
};





