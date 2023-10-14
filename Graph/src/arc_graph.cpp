#include "arc_graph.hpp"


ArcGraph::ArcGraph(const size_t n_vertices) : vertices_count_(n_vertices) {}

ArcGraph::ArcGraph(const IGraph* graph) : ArcGraph(graph->VerticesCount()) {
    std::vector<size_t> vertices;
    for (size_t from = 0; from < graph->VerticesCount(); ++from) {
        graph->GetNextVertices(from, vertices);
        for (size_t to = 0; to < vertices.size(); ++to)
            AddEdge(from, vertices[to]);
            
        vertices.clear();
    }
}

void ArcGraph::AddEdge(size_t from, size_t to) {
    edges_.push_back(std::make_pair(from, to));
}

size_t ArcGraph::VerticesCount() const noexcept {
    return vertices_count_;
}

void ArcGraph::GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t i = 0; i < edges_.size(); ++i) {
        if (edges_[i].first == vertex)
            vertices.push_back(edges_[i].second);
    }
}

void ArcGraph::GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t i = 0; i < edges_.size(); ++i) {
        if (edges_[i].second == vertex)
            vertices.push_back(edges_[i].first);
    } 
}

