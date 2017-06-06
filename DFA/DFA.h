#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>

namespace DFA {
    struct state {
        int         m_id;
        std::string m_name;
        int get_id() const{
            return m_id;
        }
        const std::string & get_name() const {
            return m_name;
        }
    };

    struct condition {
        int         m_id;
        std::string m_name;
        bool        m_flag;
        int get_id() const {
            return m_id;
        }
        const std::string & get_name() const {
            return m_name;
        }
    };
    template<typename T>
    class _id_name_map {
    public:
        using type = T;
    protected:
        std::vector<type>                     m_container;
        std::unordered_map<int, int>          m_idmap;
        std::unordered_map<std::string , int> m_namemap;

        template<typename Key, typename Value>
        static Value * null_if_existent(std::unordered_map<Key, int> & map, std::vector<Value> & vet, const Key & k) {
            auto index = map.find(k);
            if (index == map.end()) {
                return nullptr;
            }
            else {
                return &vet[index->second];
            }
        }
    public:
        void force_add(const type & c) {
            int index = -1;
            auto old_pos = m_idmap.find(c.get_id());
            if (old_pos != m_idmap.end()) {
                index = old_pos->second;
                m_container[index].~type();
                m_container[index] = c;
            }
            else {
                index = m_container.size();
                m_container.push_back(c);
            }
            m_idmap[c.get_id()]     = index;
            m_namemap[c.get_name()] = index;
        }
        bool add(const type & c) {
            if (m_idmap.find(c.get_id()) == m_idmap.end()) {
                force_add(c);
                return true;
            }
            return false;
        }
        const type * operator[](int id) const {
            return null_if_existent(m_idmap, m_container, id);
        }
        type * operator[](int id) {
            return null_if_existent(m_idmap, m_container, id);
        }
        const type * operator[](const std::string & name) const {
            return null_if_existent(m_namemap, m_container, name);
        }
        type * operator[](const std::string & name) {
            return null_if_existent(m_namemap, m_container, name);
        }
    };

    class rule : public _id_name_map<condition>{
    public:
        bool match(const condition & c) const {
            auto command = m_idmap.find(c.m_id);
            if (command == m_idmap.end()) {
                return false;
            }
            return m_container[command->second].m_flag == c.m_flag;
        }
    };

    struct context {
        std::vector<condition> m_conds;
    };

    struct edge{
        state m_next;
        rule  m_rule;
        int   priority;
    };

    class node {
    private:
        state             m_state;
        std::vector<edge> m_edges;
    public:
        
        node(const state & s) : m_state(s) {
        }
        int get_id() const{
            return m_state.get_id();
        }
        const std::string & get_name() const {
            return m_state.get_name();
        }
        void add_edge( const state& s,const rule & r, int p) {
            m_edges.push_back(edge{s, r, p});
        }
        template<typename policy> 
        const state * jump(const context & c) const{
            const state * candidate = nullptr;
            policy pl;
            for (auto & e : m_edges) {
                candidate = pl(c, e);
            }
            return candidate;
        }
    };
    struct  priority_match{
    private:
        int           m_max_priority;
        const state * m_candidate;
    public:
        priority_match() : 
            m_max_priority(std::numeric_limits<int>().min()),
            m_candidate(nullptr) {}
        const state * operator()(const context & c, const edge & e) {
            for (auto & cond : c.m_conds) {
                if (e.m_rule.match(cond) && e.priority > m_max_priority) {
                    m_max_priority = e.priority;
                    m_candidate = &e.m_next;
                    break;
                }
            }
            return m_candidate;
        }
    };
    struct  similarity_match{
    private:
        size_t        m_max_similarity;
        const state * m_candidate;
    public:
        similarity_match() : 
            m_max_similarity(std::numeric_limits<size_t>().min()),
            m_candidate(nullptr) {}
        const state * operator()(const context & c, const edge & e) {
            for (auto & cond : c.m_conds) {
                if ( ! e.m_rule.match(cond)) {
                    return nullptr;
                }
            }
            if (c.m_conds.size() > m_max_similarity) {
                m_max_similarity = c.m_conds.size();
                m_candidate = &e.m_next;
            }
            return m_candidate;
        }
    };
    
    class graph : public _id_name_map<node>{
    };
}