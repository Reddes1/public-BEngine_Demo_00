//*********************************************************************************\\
//  
// Dedicated manager for graphics related resources, functionality and support.
// Stores and manages several different DX12 & DirectXTK12 graphics resources
// like RootSigs, Spritebatches & fonts + more. For Heaps, See Mgr_TextureManager.
// 
// Additional behaviour also includes "Render Grouping", allowing Actor2D_Interface
// based classes to submitted to specific batches for the frame, allowing for 
// greater control over behaviour dictated rendering.
//
// Open to modification, specifically in regard to RootSignature creation
// (See BuildRootSignature_Default in how RootSignature are built in relation to
// how they are stored/accessed).
// 
//*********************************************************************************\\

#pragma once

#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "CommonStates.h"
#include "GraphicsMemory.h"
#include "Effects.h"
#include "..\External_Dependencies\DirectXTK12\Src\d3dx12.h"
#include <array>

#include "Types/SpriteBatch_Wrapper.h"

//
//Forward Declarations
//
class Actor2D_Interface;
struct System;

class Mgr_Graphics
{
public:

    //////////////////////////////
    /// Enums, Types & Statics ///
    //////////////////////////////

    const static int GROUP_RESERVE_COUNT = 4048;

    ////////////////////
    /// Constructors ///
    ////////////////////
    
    Mgr_Graphics(ID3D12Device* d3dDevice);
    ~Mgr_Graphics() {}

    //////////////////
    /// Operations ///
    //////////////////
    
    //
    //Inits & Clears
    //

    unsigned InsertNewRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSig);


    //Adds new spritebatch (wrapper data version) to container, returning current index
    unsigned InsertNewSpritebatch(W_SpriteBatch* data);
    unsigned InsertNewSpritebatch(std::unique_ptr<W_SpriteBatch>& data);

    //
    //Render Groups
    //

    /*
        Clears any current groups and syncs count to number of spritebatches.
        Best called after all spritebatches are created so that containers sync correctly.
        Should only be called outside of regular loop as this clears all existing containers
    */
    void SyncRenderGroupCount();

    //Submits the given actor to target render group (index should sync with target spritebatch)
    void SubmitToRenderGroup(unsigned index, Actor2D_Interface* actor);

    /*
        Starts/ends render cycle for the target batch and matching render group.
        Only calls the basic behaviour and doesn't run custom functionality
        (Access batch directly for this if its been set).
    */
    void DrawBatch(System& sys, unsigned index, ID3D12GraphicsCommandList* cmdList);
   

    //Clears all render groups of currently held actors (should be called post render)
    void ClearRenderGroups();

    //
    //Supports
    //

    //Creates 6 common static samplers for use in RootSignature construction (or elsewhere)
    std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

    //
    //Root Signature Constructors
    //

    /*
        This is the system default root signature. It approximately matches the Spritebatch
        signature with some key differences:
        - Static samplers are used instead of Spritebatch samplers. This means slot 2 is NOT
        used for that purpose and SB samplers should not be used with this signature.
        - Slot 2 is now an open and flexible const buffer slot that should be used for per-frame
        buffer usage.
        - Is open to custom VS and PS configs. However, VS in must match Spritebatch expectations.
    */
    bool BuildRootSig_Default(ID3D12Device* d3dDevice, Microsoft::WRL::ComPtr<ID3D12RootSignature>* rootSig);

    /////////////////
    /// Accessors ///
    /////////////////

    //
    //Regular accessors
    //

    DirectX::GraphicsMemory* GetGraphicsMemory() { return m_GraphicsMemory.get(); }
    DirectX::CommonStates* GetCommonStates() { return m_CommonStates.get(); }
    
    //
    //Accessors via index
    //


    ID3D12RootSignature* GetRootSignature(unsigned index);
    W_SpriteBatch* GetSpritebatch(unsigned index);
    std::vector<Actor2D_Interface*>& GetRenderGroup(unsigned index);
    DirectX::SpriteFont* GetSpritefont(unsigned index);

    //
    //Container accessors
    //

    std::vector<std::unique_ptr<W_SpriteBatch>>& GetSpritebatches() { return m_Spritebatches; }
    std::vector<std::unique_ptr<DirectX::SpriteFont>>& GetSpritefonts() { return m_Spritefonts; }

private:

    //////////////////
    /// Operations ///
    //////////////////


    ////////////
    /// Data ///
    ////////////

    //
    //Container-based resources
    //

    //Projects root signature defintions
    std::vector<Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_RootSigs;
    //Wrapped up spritebatches with associated data
    std::vector<std::unique_ptr<W_SpriteBatch>> m_Spritebatches;
    //Like spritebatches, but with drawable fonts/glyphs
    std::vector<std::unique_ptr<DirectX::SpriteFont>> m_Spritefonts;

    /*
        Represents an alternate method of rendering via grouping. Allows for Actors to be submitted
        to a render groups, and then the group can be rendered at once with the appropriate call.
        Groups are aligned to spritebatch container (e.g. Index 0 = SB Index 0). Containers are
        cleared after being rendered.
    */
    std::vector<std::vector<Actor2D_Interface*>> m_RenderGroups;

    //
    //Other Resources
    //

    //XTK support resource for setting up Spritebatches/fonts
    std::unique_ptr<DirectX::GraphicsMemory> m_GraphicsMemory;
    //DirectXTK common sampler states
    std::unique_ptr<DirectX::CommonStates> m_CommonStates;

    ///////////////
    /// WIP/NYI ///
    ///////////////

    //Allows for simple multi-texturing with spritebatch (NYI)
    std::unique_ptr<DirectX::NormalMapEffect> m_NormalMapEffect;
};