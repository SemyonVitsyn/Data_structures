#pragma once

#include "igraph.hpp"


class MatrixGraph : public IGraph {

public:
    MatrixGraph(const size_t n_vertices = 0);
    MatrixGraph(const IGraph* graph);

    ~MatrixGraph() = default;

    void AddEdge(size_t from, size_t to) override;
    size_t VerticesCount() const noexcept override;
    void GetNextVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;
    void GetPrevVertices(size_t vertex, std::vector<size_t>& vertices) const noexcept override;

    MatrixGraph(const MatrixGraph& ) = delete;
    MatrixGraph& operator=(const MatrixGraph& ) = delete;

private:
    std::vector<std::vector<bool>> matrix_;

};