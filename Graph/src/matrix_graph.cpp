#include "matrix_graph.hpp"


MatrixGraph::MatrixGraph(const size_t n_vertices) {
    for (size_t i = 0; i < n_vertices; ++i)
        matrix_.push_back(std::vector<bool>(n_vertices, false));
}

MatrixGraph::MatrixGraph(const IGraph* graph) : MatrixGraph(graph->VerticesCount()) {
    std::vector<size_t> vertices;
    for (size_t from = 0; from < graph->VerticesCount(); ++from) {
        graph->GetNextVertices(from, vertices);
        for (size_t to = 0; to < vertices.size(); ++to)
            AddEdge(from, vertices[to]);
            
        vertices.clear();
    }
}

void MatrixGraph::AddEdge(size_t from, size_t to) {
    matrix_[from][to] = true;
}

size_t MatrixGraph::VerticesCount() const noexcept {
    return matrix_.size();
}

void MatrixGraph::GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t i = 0; i < matrix_.size(); ++i) {
        if (matrix_[vertex][i])
            vertices.push_back(i);
    }
}

void MatrixGraph::GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept {
    for (size_t i = 0; i < matrix_.size(); ++i) {
        if (matrix_[i][vertex])
            vertices.push_back(i);
    } 
}

