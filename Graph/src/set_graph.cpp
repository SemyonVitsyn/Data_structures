#include "set_graph.hpp"


SetGraph::SetGraph(const size_t n_vertices) {
    for (size_t i = 0; i < n_vertices; ++i)
        graph_.push_back(std::unordered_set<size_t>());
}

SetGraph::SetGraph(const IGraph* graph) : SetGraph(graph->VerticesCount()) {
    std::vector<size_t> vertices;
    for (size_t from = 0; from < graph->VerticesCount(); ++from) {
        graph->GetNextVertices(from, vertices);
        for (size_t to = 0; to < vertices.size(); ++to)
            this->AddEdge(from, vertices[to]);
            
        vertices.clear();
    }
}

void SetGraph::AddEdge(size_t from, size_t to) {
    graph_[from].insert(to);
}

size_t SetGraph::VerticesCount() const noexcept {
    return graph_.size();
}

void SetGraph::GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (std::unordered_set<size_t> :: const_iterator i = graph_[vertex].begin(); i != graph_[vertex].end(); ++i)
        vertices.push_back(*i);
}

void SetGraph::GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t from = 0; from < graph_.size(); ++from)
        if (graph_[from].find(vertex) != graph_[from].end())
            vertices.push_back(from);
}

