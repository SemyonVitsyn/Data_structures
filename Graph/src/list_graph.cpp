#include "list_graph.hpp"


ListGraph::ListGraph(const size_t n_vertices) {
    for (size_t i = 0; i < n_vertices; ++i)
        graph_.push_back(std::vector<size_t>());
}

ListGraph::ListGraph(const IGraph* graph) : ListGraph(graph->VerticesCount()) {
    std::vector<size_t> vertices;
    for (size_t from = 0; from < graph->VerticesCount(); ++from) {
        graph->GetNextVertices(from, vertices);
        for (size_t to = 0; to < vertices.size(); ++to)
            AddEdge(from, vertices[to]);
            
        vertices.clear();
    }
}

void ListGraph::AddEdge(size_t from, size_t to) {
    graph_[from].push_back(to);
}

size_t ListGraph::VerticesCount() const noexcept {
    return graph_.size();
}

void ListGraph::GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t i = 0; i < graph_[vertex].size(); ++i) {
            vertices.push_back(graph_[vertex][i]);
    }
}

void ListGraph::GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t from = 0; from < graph_.size(); ++from)
        for (size_t i = 0; i < graph_[from].size(); ++i)
            if (graph_[from][i] == vertex) 
                vertices.push_back(from);
}
