#include "SpriteBatch_Wrapper.h"

#include "Actors/Actor2D_Interface.h"

void W_SpriteBatch::Render(System& sys, std::vector<Actor2D_Interface*>& actors)
{
	for (auto& a : actors)
		a->Render(sys, m_Batch.get());
}

void W_SpriteBatch::Render(System& sys, Actor2D_Interface* actor)
{
	actor->Render(sys, m_Batch.get());
}
