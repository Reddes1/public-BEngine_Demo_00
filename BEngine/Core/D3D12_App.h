//*********************************************************************************\\
// 
// Parent class for D3D12 Application.A "Game" class should be derived from this object to
// fulfil and provide remaining contextual behaviour.
// 
// Primarily configures Windows side setup and minimal D3D12 startup behaviour.
// Uses Singleton Pattern.
// 
//*********************************************************************************\\

#pragma once
//Library Includes
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <string>

//Utils
#include "Utils/Utils_D3D.h"

//Engine Includes
#include "Timing/GameTimer.h"	//Game DT + Total Time

class D3D12_App
{
protected:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Window Data
	struct WindowData
	{
		////////////
		/// Data ///
		////////////

		std::string m_WindowName = "BEngine_Demo_1.0";	//Window Title/Caption
		DWORD       m_WinStyle = (WS_TILEDWINDOW);		//Window Style
		HINSTANCE   m_AppInst = nullptr;				//Application instance handle
		HWND        m_HWND = nullptr;					//Main window handle
		int         m_CurrWidth = 1920;					//Current Window Width
		int         m_CurrHeight = 1080;				//Current Window Height
		int         m_NativeWidth = 1920;				//Original Window Width
		int         m_NativeHeight = 1080;				//Original Window Height
	};
	//Window Flags
	struct WindowFlags
	{
		WindowFlags()
			:m_AppPaused(false), m_AppMinimized(false), m_AppMaximized(false),
			m_AppResizing(false), m_AppFullscreenState(false), m_Pad_00(false),
			m_Pad_01(false), m_Pad_02(false)
		{}

		bool m_AppPaused : 1;          //Is the application paused?
		bool m_AppMinimized : 1;       //Is the application minimized?
		bool m_AppMaximized : 1;       //Is the application maximized?
		bool m_AppResizing : 1;        //Are the resize bars being dragged?
		bool m_AppFullscreenState : 1; //Fullscreen enabled?
		bool m_Pad_00 : 1;
		bool m_Pad_01 : 1;
		bool m_Pad_02 : 1;
	};

	////////////////////
	/// Constructors ///
	////////////////////

	//Main constructor, stores instance + access pointer
	D3D12_App(HINSTANCE hInstance);
	D3D12_App(const D3D12_App& rSide) = delete;
	D3D12_App& operator=(const D3D12_App& rSide) = delete;
	virtual ~D3D12_App();

	//////////////////
	/// Operations ///
	//////////////////

	//Default init function (using internal setup)
	//If using, Call only ONCE in child classes init function
	bool InitialiseD3D();
	//Releases D3D components
	bool ReleaseD3D(bool extraReporting);
	//Clears the command queue
	void FlushCommandQueue();
	//Call whenever a resize is needed
	virtual void OnResize();

	/////////////////
	/// Accessors ///
	/////////////////

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHeapStart() const { return m_DSVHeap->GetCPUDescriptorHandleForHeapStart(); }

public:

	////////////////
	/// Virtuals ///
	////////////////

	//Main Update Window
	virtual void Update() = 0;
	//Main Render Window
	virtual void Render() = 0;

	//Windows Message Handler (Must be defined by child, redirects to this from XX)
	virtual LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	//Called after WM_CLOSE message passed (first stage when user/action has signalled to close the window)
	virtual void HandleWindowClosing() {}
	//Called after WM_DESTROY message passed (final stage before window is properly destroyed and released)
	virtual void HandleWindowDestroy() {}


	//////////////////
	/// Operations ///
	//////////////////

	//Starts main game loop (call after creation ONCE, loop will control exit conditions)
	int RunApp();

	/////////////////
	/// Accessors ///
	/////////////////

	//Primary App Accessor
	static D3D12_App* GetApp() { return m_App; }

	Microsoft::WRL::ComPtr<ID3D12Device>& GetD3DDevice() { return m_D3DDevice; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GetCommandList() { return m_CommandList; }

	WindowData& GetWindowData() { return m_WinData; }
	GameTimer& GetGameTime() { return m_Timer; }
	
	//
	//Utilities
	//

	//Generates an adjustment multiplier based on the current window size against the native resolution
	Vec2 GetWinAdjustMult();

private:

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Default Init Functions, Maybe be overridden with custom from child class (Do NOT call InitialiseD3D if so)
	//
	
	//Initialise Window
	bool InitMainWindow(int nativeX, int nativeY);
	//Initialise D3D
	bool InitD3D();
	//Create CO, SC, D.Heaps
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	//Adapter setup
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	//
	//Runtime behaviour
	//

	//Update game time and calc Delta Time
	void CalculateFrameStats();

	//
	//Other
	// 


protected:

	////////////
	/// Data ///
	////////////

	//Singleton access pointer
	static D3D12_App* m_App;
	//Window Data
	WindowData m_WinData;
	//And Window Flags
	WindowFlags m_WinFlags;
	//Track gametime and delta time;
	GameTimer m_Timer;

	//
	//D3D12 Data
	//

	//Back buffer and depth stencil formats (alter in child as required)
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	UINT m_4xMsaaQuality = 1;
	bool m_Enable4xMsaa = false;		//Set true to use 4X MSAA (§4.1.8).  The default is false.

	//Swap chain buffer values
	static const int SWAP_CHAIN_BUFFER_COUNT = 2;
	int m_CurrentBackBuffer = 0;

	//Main Device, Factory and Swap Chain
	Microsoft::WRL::ComPtr<ID3D12Device> m_D3DDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_DXGIFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

	//Command Queue/Allocator/List
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCommandListAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	//Fence (for syncing)
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_CurrentFence = 0;

	//Swap Chain Buffer & Depth Stencil buffer
	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SWAP_CHAIN_BUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	//Render Target View & Depth Stencil View
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;

	//Viewport and Scissor
	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	//Descriptor sizes
	UINT m_RTVDescriptorSize = 0;
	UINT m_DSVDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;
};