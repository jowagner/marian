#include "decoder.h"

namespace amunmt {
namespace FPGA {

void Decoder::EmptyState(mblas::Matrix& State,
                const mblas::Matrix& SourceContext,
                size_t batchSize,
                const Array<int>& batchMapping)
{
  rnn1_.InitializeState(State, SourceContext, batchSize, batchMapping);
  alignment_.Init(SourceContext);
}

void Decoder::EmptyEmbedding(mblas::Matrix& Embedding, size_t batchSize) {
  Embedding.Resize(batchSize, embeddings_.GetCols());
  mblas::Fill(Embedding, 0);
}

void Decoder::Decode(mblas::Matrix& NextState,
              const mblas::Matrix& State,
              const mblas::Matrix& Embeddings,
              const mblas::Matrix& SourceContext,
              const Array<int>& mapping,
              const std::vector<size_t>& beamSizes)
{
  //std::cerr << std::endl;

  GetHiddenState(HiddenState_, State, Embeddings);
  GetAlignedSourceContext(AlignedSourceContext_, HiddenState_, SourceContext, mapping, beamSizes);
  std::cerr << "AlignedSourceContext_=" << AlignedSourceContext_.Debug(1) << std::endl;

  GetNextState(NextState, HiddenState_, AlignedSourceContext_);
  std::cerr << "NextState=" << NextState.Debug(1) << std::endl;

  GetProbs(NextState, Embeddings, AlignedSourceContext_);


}

void Decoder::GetHiddenState(mblas::Matrix& HiddenState,
                    const mblas::Matrix& PrevState,
                    const mblas::Matrix& Embedding) {
  rnn1_.GetNextState(HiddenState, PrevState, Embedding);
}

void Decoder::GetAlignedSourceContext(mblas::Matrix& AlignedSourceContext,
                              const mblas::Matrix& HiddenState,
                              const mblas::Matrix& SourceContext,
                              const Array<int>& mapping,
                              const std::vector<size_t>& beamSizes)
{
  alignment_.GetAlignedSourceContext(AlignedSourceContext, HiddenState, SourceContext,
                                     mapping, beamSizes);

}

void Decoder::GetNextState(mblas::Matrix& State,
                  const mblas::Matrix& HiddenState,
                  const mblas::Matrix& AlignedSourceContext)
{
  rnn2_.GetNextState(State, HiddenState, AlignedSourceContext);
}

void Decoder::GetProbs(const mblas::Matrix& State,
              const mblas::Matrix& Embedding,
              const mblas::Matrix& AlignedSourceContext)
{
  softmax_.GetProbs(Probs_, State, Embedding, AlignedSourceContext);
}

} // namespace
}
