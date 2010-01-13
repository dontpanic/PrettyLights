#pragma once

const HDC NULL_HDC = 0;

#include <atlbase.h>
#include <memory>
#include <ddraw.h>
#pragma comment(lib,"ddraw.lib")


BOOL DXErrorString(HRESULT hResult, LPTSTR lpszErrorBuff, DWORD cchError);

struct XRect : tagRECT
{
	XRect()
	{	}
	XRect(int l, int t, int r, int b)
	{ left = l; top = t; right = r; bottom = b; }
	XRect(const RECT& srcRect)
	{ ::CopyRect(this, &srcRect); }
	XRect(LPCRECT lpSrcRect)
	{ ::CopyRect(this, lpSrcRect); }
	XRect(POINT point, SIZE size)
	{ right = (left = point.x) + size.cx; bottom = (top = point.y) + size.cy; }
	XRect(POINT topLeft, POINT bottomRight)
	{   left = topLeft.x; top = topLeft.y;right = bottomRight.x; bottom = bottomRight.y; }
	
	int Width() const
	{ return right - left; }

	int Height() const
	{ return bottom - top; }
	
	operator LPRECT()
	{ return this; }
	
	operator LPCRECT() const
	{ return this; }
	
	BOOL IsRectEmpty() const
	{ return ::IsRectEmpty(this); }
	
	BOOL IsRectNull() const
	{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }

	BOOL PtInRect(POINT point) const
	{ return ::PtInRect(this, point); }

	void operator=(const RECT& srcRect)
	{ ::CopyRect(this, &srcRect); }
	
	BOOL operator==(const RECT& rect) const
	{ return ::EqualRect(this, &rect); }
	
	BOOL operator!=(const RECT& rect) const
	{ return !::EqualRect(this, &rect); }

};

// this builds a 16 bit color value in 5.5.5 format (1-bit alpha mode)
#define _RGB16BIT555(r,g,b) ((b%32) + ((g%32) << 5) + ((r%32) << 10))

// this builds a 16 bit color value in 5.6.5 format (green dominate mode)
#define _RGB16BIT565(r,g,b) ((b%32) + ((g%64) << 5) + ((r%32) << 11))

interface ClrConverter
{
	virtual DWORD Convert(int r, int g, int b) = 0;
};

struct CRGB8BIT : ClrConverter
{
	DWORD lastasked_,lastgot_;
	CComPtr<IDirectDrawSurface> srfc_;

	CRGB8BIT(IDirectDrawSurface* srfc)
		:srfc_(srfc), lastasked_(-1)
	{	}

	DWORD Convert(int r, int g, int b)
	{
		COLORREF dwGDIColor = RGB(r,g,b);
		if(lastasked_==dwGDIColor)
			return lastgot_;

		lastasked_ = dwGDIColor;
		COLORREF       rgbT;
		//  Use GDI SetPixel to color match for us
		HDC hdc;
		if( dwGDIColor != CLR_INVALID && srfc_->GetDC(&hdc) == DD_OK)
		{
			rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
			SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
			srfc_->ReleaseDC(hdc);
		}

		// Now lock the surface so we can read back the converted color
		DWORD dw = CLR_INVALID;
		DDSURFACEDESC  ddsd;
		ddsd.dwSize = sizeof(ddsd);
		HRESULT hr = srfc_->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL );
		if(SUCCEEDED(hr))
		{
			dw = *(DWORD *) ddsd.lpSurface; 
//			if( ddsd.ddpfPixelFormat.dwRGBBitCount < 32 ) // Mask it to bpp
				dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  
			srfc_->Unlock(NULL);
		}

		//  Now put the color that was there back.
		if(dwGDIColor != CLR_INVALID && srfc_->GetDC(&hdc) == DD_OK)
		{
			SetPixel( hdc, 0, 0, rgbT );
			srfc_->ReleaseDC(hdc);
		}

		lastgot_ = dw;
		return dw;    
	}

};

struct CRGB24BIT : ClrConverter
{
	DWORD Convert(int r, int g, int b)
	{
		return ((b) + ((g) << 8) + ((r) << 16));
	}
};

struct CRGB32BIT : ClrConverter
{
	DWORD Convert(int r, int g, int b)
	{
		int a = 0;
		return ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24));
	}
};

struct CRGBTo16BIT565 : ClrConverter
{
	DWORD Convert(int r, int g, int b)
	{
		const int scale5 = 256/32;
		const int scale6 = 256/64;
		return _RGB16BIT565( r / scale5, g / scale6, b / scale5 );
	}
};

struct CRGBTo16BIT555 : ClrConverter
{
	DWORD Convert(int r, int g, int b)
	{
		const int scale5 = 256/32;
		return _RGB16BIT565( r / scale5, g / scale5, b / scale5 );
	}
};

/*
	encapsulates a DDSURFACEDESC struct intialisation
*/
struct CDDSURFACEDESC:DDSURFACEDESC
{
	CDDSURFACEDESC(DWORD Flags, DWORD Caps, int Width = 0, int Height = 0)
	{
		memset(this, 0,sizeof(CDDSURFACEDESC));
		this->dwSize = sizeof(DDSURFACEDESC);	
		this->dwFlags = Flags; 
		this->ddsCaps.dwCaps = Caps;
		if(Width > 0 && Height > 0)
		{
			this->dwHeight = Height; 
			this->dwWidth = Width; 
		}
	}
};

/*
	encapsulates a DDPIXELFORMAT struct intialisation
	and the pxiel format of a DirectDraw surface 
*/
struct CDDPIXELFORMAT:DDPIXELFORMAT
{
	DWORD bitformat_;
	CDDPIXELFORMAT(IDirectDrawSurface* Surface)
		:bitformat_(0)
	{
		memset(this, 0,sizeof(DDPIXELFORMAT)); 
		this->dwSize = sizeof(DDPIXELFORMAT);
		if(SUCCEEDED(Surface->GetPixelFormat(this)))
		{
			bitformat_ = this->dwRGBBitCount; 
		}
	}

	bool IsValid()
	{
		return bitformat_ != 0?true:false;
	}

	/*
		whats the format of the pixels on a
		8/16/24/32 bit surface
	*/
	DWORD GetBitCount() const 
	{
		return bitformat_;
	}
	/*
		are we in 16/24/32 bit mode
	*/
	bool InRGBMode() const 
	{
		return ( (this->dwFlags & DDPF_RGB) && !(this->dwFlags & DDPF_PALETTEINDEXED8)  ) ? true: false;
	}
	/*
		are we in 8 bit mode and therefore using a palette
	*/
	bool InPaletteMode() const 
	{
		return ((this->dwFlags & DDPF_PALETTEINDEXED8) || (this->dwFlags & DDPF_PALETTEINDEXEDTO8))? true: false;
	}
};

/*
	use this trait as a template parameter CDXSurfaceMgr if 
	you want it (CDXSurfaceMgr) to call begin/end paint
*/
struct TDoBeginPaint
{
    PAINTSTRUCT m_ps;

	HDC BeginPaint(HWND h)
	{
		return ::BeginPaint(h, &m_ps);
	}

	BOOL EndPaint(HWND h)
	{
		return ::EndPaint(h, &m_ps);
	}
};

/*
	use this trait as a template parameter CDXSurfaceMgr if 
	_dont_ you want it (CDXSurfaceMgr) to call begin/end paint
	usefull in mfc were you are drawing in the OnDraw method and 
	not using CDC to do the beginpaint/endpaint calls for you
*/
struct TDontBeginPaint
{
    PAINTSTRUCT m_ps;

	HDC hdc_;

	TDontBeginPaint()
		:hdc_(0)
	{	}

	HDC BeginPaint(HWND h)
	{ return hdc_;	}

	BOOL EndPaint(HWND h)
	{ return false;	}
};

template <class TBEGINPAINT = TDoBeginPaint>
class CDXSurfaceMgr : public TBEGINPAINT
{
	CComPtr<IDirectDraw> directdraw_;
	CComPtr<IDirectDrawSurface> primarysurface_, secondarysurface_;
	CComPtr<IDirectDrawClipper> clipper_;

	HWND window_;

	std::auto_ptr<ClrConverter> rgbconverter_;

	HDC current_hdc_;
	XRect current_rect_;

	IDirectDrawSurface* CreateSecondarySurface(const int Width, const int Height)
	{
		if(NULL!=primarysurface_.p)
		{
			IDirectDrawSurface* ss =NULL;
			CDDSURFACEDESC dds(DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH, DDSCAPS_OFFSCREENPLAIN, Width, Height);
			HRESULT hr = directdraw_->CreateSurface(&dds,&ss, NULL);
			if(SUCCEEDED(hr))
				return ss;
		}
		return NULL;
	}

	void fillclr(IDirectDrawSurface* surface, COLORREF BackFillColor)
	{
		DDBLTFX bltfx; memset(&bltfx, 0,sizeof(DDBLTFX)); bltfx.dwSize = sizeof(DDBLTFX);
		// convert the rgb given into what ever pixel format we may have
		bltfx.dwFillColor = rgbconverter_->Convert(GetRValue(BackFillColor),
												   GetGValue(BackFillColor),
												   GetBValue(BackFillColor));
		HRESULT hr = surface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL|DDBLT_WAIT,&bltfx);
	}

	BOOL ClientToScreen(LPRECT lpRect) const
	{
		if(!::ClientToScreen(window_, (LPPOINT)lpRect))
			return FALSE;
		return ::ClientToScreen(window_, ((LPPOINT)lpRect)+1);
	}

	bool SetPixel(int x, int y, UCHAR* pb, DDSURFACEDESC& ddsd, int Clr)
	{
		*((USHORT *)(pb + x*2 + y*ddsd.lPitch)) = Clr;
		return true;
	}

	IDirectDrawPalette* Build256Palette()
	{
		IDirectDrawPalette* ddp = 0;
		PALETTEENTRY pal[256];
		
		// setup static windows entries
		for(int i =0; i<10 ;i++)
		{
			//the first 10
			pal[i].peFlags = PC_EXPLICIT;
			pal[i].peRed = i;
			pal[i].peGreen = pal[i].peBlue = 0;

			// the last ten
			pal[i+246].peFlags = PC_EXPLICIT;
			pal[i+246].peRed = i+246;
			pal[i+246].peGreen = pal[i].peBlue = 0;
		}

		// default everything else	
		for(i=10; i<246 ;i++)
		{
			pal[i].peFlags = PC_NOCOLLAPSE;
			pal[i].peRed = pal[i].peGreen = pal[i].peBlue = 27;
		}

		HRESULT hr =directdraw_->CreatePalette(DDPCAPS_8BIT, pal, &ddp, NULL);
		return ddp;
	}

public:

	bool IsValid()
	{
		return (primarysurface_.p != NULL) ? true:false;
	}

	/*
		set-up the primary surface its cooperation level and the 
		clipper, then initialise the color conversion structures
	*/
	bool Initialise(HWND Window)
	{
		if(NULL==Window) return false;

		window_ = Window;
		
		// create DirectDrawObject
		HRESULT hr = ::DirectDrawCreate(NULL, &directdraw_.p,NULL);
		if (FAILED(hr)) return false;
		hr = directdraw_->SetCooperativeLevel(window_,DDSCL_NORMAL);
		if (FAILED(hr)) return false;
		// create primary surface
		CDDSURFACEDESC dds(DDSD_CAPS, DDSCAPS_PRIMARYSURFACE);
		hr = directdraw_->CreateSurface(&dds,&primarysurface_.p, NULL);
		if (FAILED(hr)) return false;
		// attach clipper to window all blits will 
		// then be clipped to the primary surface
		hr = directdraw_->CreateClipper(0, &clipper_.p, NULL);
		if (FAILED(hr)) return false;
		hr = clipper_->SetHWnd(0, window_);
		if (FAILED(hr)) return false;
		// attach clipper to primary surface
		hr = primarysurface_->SetClipper(clipper_);
		if (FAILED(hr)) return false;

		// work out the pixel format for colour conversions
		CDDPIXELFORMAT pf(primarysurface_.p);
		if(pf.IsValid())
		{
			if(pf.InRGBMode())
			{
				DWORD tst = pf.GetBitCount();
				if(15 == tst) // 16 bit 555
				{
					std::auto_ptr<ClrConverter> rgbconverter(new CRGBTo16BIT555);
					rgbconverter_ = rgbconverter;
				}
				if(16 == tst) // 16 bit 565
				{
					std::auto_ptr<ClrConverter> rgbconverter(new CRGBTo16BIT565);
					rgbconverter_ = rgbconverter;
				}
				if(24 == tst) // 24 bit
				{
					std::auto_ptr<ClrConverter> rgbconverter(new CRGB24BIT);
					rgbconverter_ = rgbconverter;
				}
				if(32 == tst) // 32 bit
				{
					std::auto_ptr<ClrConverter> rgbconverter(new CRGB32BIT);
					rgbconverter_ = rgbconverter;
				}
			}
			else if(pf.InPaletteMode())
			{
				std::auto_ptr<ClrConverter> rgbconverter(new CRGB8BIT(primarysurface_));
				rgbconverter_ = rgbconverter;
			}
			else
			{
				ATLTRACE("u r living on borrowed time - please retire now\n");
			}
		}

		return true;
	}

	/*
		if needed create a new secondary surface
		and get the DC off it and return it
	*/
	HDC& BeginPaint(COLORREF BackFillColor = 0)
	{
		current_hdc_ = NULL_HDC;
		if(!IsValid()) return current_hdc_;

		TBEGINPAINT::BeginPaint(window_);

		XRect client; ::GetClientRect(window_, &client); ClientToScreen(&client);
 
		// only create new secondary surface if the window has changed size
		if(current_rect_!=client) 
		{
			secondarysurface_ = CreateSecondarySurface(client.Width(), client.Height());
			if(NULL==secondarysurface_.p) return current_hdc_;
			current_rect_ = client;
		}
		// fill the secondary surface 
		fillclr(secondarysurface_, BackFillColor);
		
		// GetDC will _lock_ the surface for us
		secondarysurface_->GetDC(&current_hdc_);
		return current_hdc_;
	}

	/*
		simply blit the secondary surface onto the primary
		one we can't use flip as we are in windowed mode 
	*/
	bool EndPaint()
	{
		if(NULL_HDC!=current_hdc_)
		{
			// ReleaseDC will unlock the surface
			secondarysurface_->ReleaseDC(current_hdc_);

			// copy all of the secondary surface onto the primary
			HRESULT hr = primarysurface_->Blt(&current_rect_, // destination
						  secondarysurface_, NULL,  // Source surface, Source rectangle NULL = entire surface
						  DDBLT_WAIT, NULL);
			if(SUCCEEDED(hr))
			{
				TBEGINPAINT::EndPaint(window_);
				return true;
			}
		}
		return false;
	}
};

typedef CDXSurfaceMgr<TDoBeginPaint> CDXSurfaceManager;
// use this one if you call your BeginPaint/EndPaint or use CDC
typedef CDXSurfaceMgr<TDontBeginPaint> CDXSurfaceManager_NBP;

static BOOL DXErrorString(HRESULT hResult, LPTSTR lpszErrorBuff, DWORD cchError)
{
	DWORD  cLen;
	LPTSTR lpszError;
	TCHAR  szMsg[1024];

	// Check parameters
	if (!lpszErrorBuff || !cchError)
	{
		// Error, invalid parameters
		return FALSE;
	}

	switch (hResult)
	{
	case DD_OK:
		// The request completed successfully.
		lpszError = TEXT("DD_OK");
		break;

	case DDERR_ALREADYINITIALIZED:
		// The object has already been initialized.
		lpszError = TEXT("DDERR_ALREADYINITIALIZED");
		break;

	case DDERR_BLTFASTCANTCLIP:
		// A DirectDrawClipper object is attached to a source surface 
		// that has passed into a call to the IDirectDrawSurface2::BltFast method. 
		lpszError = TEXT("DDERR_BLTFASTCANTCLIP");
		break;

	case DDERR_CANNOTATTACHSURFACE:
		// A surface cannot be attached to another requested surface. 
		lpszError = TEXT("DDERR_CANNOTATTACHSURFACE");
		break;

	case DDERR_CANNOTDETACHSURFACE:
		// A surface cannot be detached from another requested surface. 
		lpszError = TEXT("DDERR_CANNOTDETACHSURFACE");
		break;

	case DDERR_CANTCREATEDC:
		// Windows cannot create any more device contexts (DCs). 
		lpszError = TEXT("DDERR_CANTCREATEDC");
		break;

	case DDERR_CANTDUPLICATE:
		// Primary and 3D surfaces, or surfaces that are 
		// implicitly created, cannot be duplicated. 
		lpszError = TEXT("DDERR_CANTDUPLICATE");
		break;

	case DDERR_CANTLOCKSURFACE:
		// Access to this surface is refused because an 
		// attempt was made to lock the primary surface without DCI support. 
		lpszError = TEXT("DDERR_CANTLOCKSURFACE"); 
		break;

	case DDERR_CANTPAGELOCK:
		// An attempt to page lock a surface failed. 
		// Page lock will not work on a display-memory 
		// surface or an emulated primary surface.
		lpszError = TEXT("DDERR_CANTPAGELOCK"); 
		break;

	case DDERR_CANTPAGEUNLOCK:
		// An attempt to page unlock a surface failed. 
		// Page unlock will not work on a display-memory 
		// surface or an emulated primary surface. 
		lpszError = TEXT("DDERR_CANTPAGEUNLOCK");
		break;

	case DDERR_CLIPPERISUSINGHWND:
		// An attempt was made to set a clip list for a DirectDrawClipper 
		// object that is already monitoring a window handle. 
		lpszError = TEXT("DDERR_CLIPPERISUSINGHWND");
		break;

	case DDERR_COLORKEYNOTSET:
		// No source color key is specified for this operation
		lpszError = TEXT("DDERR_COLORKEYNOTSET");
		break;

	case DDERR_CURRENTLYNOTAVAIL:
		// No support is currently available. 
		lpszError = TEXT("DDERR_CURRENTLYNOTAVAIL");
		break;

	case DDERR_DCALREADYCREATED:
		// A device context (DC) has already been returned for this surface. 
		// Only one DC can be retrieved for each surface. 
		lpszError = TEXT("DDERR_DCALREADYCREATED");
		break;

	case DDERR_DIRECTDRAWALREADYCREATED:
		// A DirectDraw object representing this driver 
		// has already been created for this process. 
		lpszError = TEXT("DDERR_DIRECTDRAWALREADYCREATED");
		break;

	case DDERR_EXCEPTION:
		// An exception was encountered while 
		// performing the requested operation. 
		lpszError = TEXT("DDERR_EXCEPTION");
		break;

	case DDERR_EXCLUSIVEMODEALREADYSET:
		// An attempt was made to set the cooperative 
		// level when it was already set to exclusive. 
		lpszError = TEXT("DDERR_EXCLUSIVEMODEALREADYSET");
		break;

	case DDERR_GENERIC:
		// There is an undefined error condition. 
		lpszError = TEXT("DDERR_GENERIC");
		break;

	case DDERR_HEIGHTALIGN:
		// The height of the provided rectangle 
		// is not a multiple of the required alignment. 
		lpszError = TEXT("DDERR_HEIGHTALIGN");
		break;

	case DDERR_HWNDALREADYSET:
		// The DirectDraw cooperative level window 
		// handle has already been set. It cannot 
		// be reset while the process has surfaces or palettes created. 
		lpszError = TEXT("DDERR_HWNDALREADYSET");
		break;

	case DDERR_HWNDSUBCLASSED:
		// DirectDraw is prevented from restoring state because the
		// DirectDraw cooperative level window handle has been subclassed. 
		lpszError = TEXT("DDERR_HWNDSUBCLASSED");
		break;

	case DDERR_IMPLICITLYCREATED:
		// The surface cannot be restored because 
		// it is an implicitly created surface. 
		lpszError = TEXT("DDERR_IMPLICITLYCREATED");
		break;

	case DDERR_INCOMPATIBLEPRIMARY:
		// The primary surface creation request 
		// does not match with the existing primary surface. 
		lpszError = TEXT("DDERR_INCOMPATIBLEPRIMARY");
		break;

	case DDERR_INVALIDCAPS:
		// One or more of the capability bits 
		// passed to the callback function are incorrect. 
		lpszError = TEXT("DDERR_INVALIDCAPS");
		break;

	case DDERR_INVALIDCLIPLIST:
		// DirectDraw does not support the provided clip list.  
		lpszError = TEXT("DDERR_INVALIDCLIPLIST");
		break;

	case DDERR_INVALIDDIRECTDRAWGUID:
		// The globally unique identifier (GUID) passed to the
		// DirectDrawCreate function is not a valid DirectDraw driver identifier. 
		lpszError = TEXT("DDERR_INVALIDDIRECTDRAWGUID");
		break;

	case DDERR_INVALIDMODE:
		// DirectDraw does not support the requested mode. 
		lpszError = TEXT("DDERR_INVALIDMODE");
		break;

	case DDERR_INVALIDOBJECT:
		// DirectDraw received a pointer that was an invalid DirectDraw object. 
		lpszError = TEXT("DDERR_INVALIDOBJECT");
		break;

	case DDERR_INVALIDPARAMS:
		// One or more of the parameters passed to the method are incorrect. 
		lpszError = TEXT("DDERR_INVALIDPARAMS");
		break;

	case DDERR_INVALIDPIXELFORMAT:
		// The pixel format was invalid as specified. 
		lpszError = TEXT("DDERR_INVALIDPIXELFORMAT");
		break;

	case DDERR_INVALIDPOSITION:
		// The position of the overlay on the destination is no longer legal. 
		lpszError = TEXT("DDERR_INVALIDPOSITION");
		break;

	case DDERR_INVALIDRECT:
		// The provided rectangle was invalid. 
		lpszError = TEXT("DDERR_INVALIDRECT");
		break;

	case DDERR_INVALIDSURFACETYPE:
		// The requested operation could not be performed
		// because the surface was of the wrong type. 
		lpszError = TEXT("DDERR_INVALIDSURFACETYPE");
		break;

	case DDERR_LOCKEDSURFACES:
		// One or more surfaces are locked, 
		// causing the failure of the requested operation. 
		lpszError = TEXT("DDERR_LOCKEDSURFACES");
		break;

	case DDERR_MOREDATA:
		// There is more data available than the specified 
		// buffer size could hold.
		lpszError = TEXT("DDERR_MOREDATA");
		break;

	case DDERR_NO3D:
		// No 3D hardware or emulation is present. 
		lpszError = TEXT("DDERR_NO3D");
		break;

	case DDERR_NOALPHAHW:
		// No alpha acceleration hardware is present or available, 
		// causing the failure of the requested operation. 
		lpszError = TEXT("DDERR_NOALPHAHW");
		break;

	case DDERR_NOBLTHW:
		// No blitter hardware is present. 
		lpszError = TEXT("DDERR_NOBLTHW");
		break;

	case DDERR_NOCLIPLIST:
		// No clip list is available. 
		lpszError = TEXT("DDERR_NOCLIPLIST");
		break;

	case DDERR_NOCLIPPERATTACHED:
		// No DirectDrawClipper object is attached to the surface object. 
		lpszError = TEXT("DDERR_NOCLIPPERATTACHED");
		break;

	case DDERR_NOCOLORCONVHW:
		// The operation cannot be carried out because 
		// no color-conversion hardware is present or available. 
		lpszError = TEXT("DDERR_NOCOLORCONVHW");
		break;

	case DDERR_NOCOLORKEY:
		// The surface does not currently have a color key. 
		lpszError = TEXT("DDERR_NOCOLORKEY");
		break;

	case DDERR_NOCOLORKEYHW:
		// The operation cannot be carried out because there 
		// is no hardware support for the destination color key. 
		lpszError = TEXT("DDERR_NOCOLORKEYHW");
		break;

	case DDERR_NOCOOPERATIVELEVELSET:
		// A create function is called without the 
		// IDirectDraw2::SetCooperativeLevel method being called. 
		lpszError = TEXT("DDERR_NOCOOPERATIVELEVELSET");
		break;

	case DDERR_NODC:
		// No DC has ever been created for this surface. 
		lpszError = TEXT("DDERR_NODC");
		break;

	case DDERR_NODDROPSHW:
		// No DirectDraw raster operation (ROP) hardware is available. 
		lpszError = TEXT("DDERR_NODDROPSHW");
		break;

	case DDERR_NODIRECTDRAWHW:
		// Hardware-only DirectDraw object creation is not possible; 
		// the driver does not support any hardware. 
		lpszError = TEXT("DDERR_NODIRECTDRAWHW");
		break;

	case DDERR_NODIRECTDRAWSUPPORT:
		// DirectDraw support is not possible with the current display driver. 
		lpszError = TEXT("DDERR_NODIRECTDRAWSUPPORT");
		break;

	case DDERR_NOEMULATION: 
		// Software emulation is not available. 
		lpszError = TEXT("DDERR_NOEMULATION");
		break;

	case DDERR_NOEXCLUSIVEMODE:
		// The operation requires the application to have 
		// exclusive mode, but the application does not have exclusive mode. 
		lpszError = TEXT("DDERR_NOEXCLUSIVEMODE");
		break;

	case DDERR_NOFLIPHW: 
		// Flipping visible surfaces is not supported. 
		lpszError = TEXT("DDERR_NOFLIPHW");
		break;

	case DDERR_NOGDI: 
		// No GDI is present. 
		lpszError = TEXT("DDERR_NOGDI");
		break;

	case DDERR_NOHWND: 
		// Clipper notification requires a window handle, 
		// or no window handle has been previously set 
		// as the cooperative level window handle. 
		lpszError = TEXT("DDERR_NOHWND");
		break;

	case DDERR_NOMIPMAPHW: 
		// The operation cannot be carried out because no 
		// mipmap texture mapping hardware is present or available. 
		lpszError = TEXT("DDERR_NOMIPMAPHW");
		break;

	case DDERR_NOMIRRORHW: 
		// The operation cannot be carried out because 
		// no mirroring hardware is present or available. 
		lpszError = TEXT("DDERR_NOMIRRORHW");
		break;

	case DDERR_NONONLOCALVIDMEM: 
		// An attempt was made to allocate non-local video memory
		// from a device that does not support non-local video memory.
		lpszError = TEXT("DDERR_NONONLOCALVIDMEM");
		break;

	case DDERR_NOOVERLAYDEST: 
		// The IDirectDrawSurface2::GetOverlayPosition method 
		// is called on an overlay that the IDirectDrawSurface2::UpdateOverlay 
		// method has not been called on to establish a destination. 
		lpszError = TEXT("DDERR_NOOVERLAYDEST");
		break;

	case DDERR_NOOVERLAYHW: 
		// The operation cannot be carried out because 
		// no overlay hardware is present or available. 
		lpszError = TEXT("DDERR_NOOVERLAYHW");
		break;

	case DDERR_NOPALETTEATTACHED: 
		// No palette object is attached to this surface. 
		lpszError = TEXT("DDERR_NOPALETTEATTACHED");
		break;

	case DDERR_NOPALETTEHW: 
		// There is no hardware support for 16- or 256-color palettes. 
		lpszError = TEXT("DDERR_NOPALETTEHW");
		break;

	case DDERR_NORASTEROPHW: 
		// The operation cannot be carried out because 
		// no appropriate raster operation hardware is present or available. 
		lpszError = TEXT("DDERR_NORASTEROPHW");
		break;

	case DDERR_NOROTATIONHW: 
		// The operation cannot be carried out because 
		// no rotation hardware is present or available. 
		lpszError = TEXT("DDERR_NOROTATIONHW");
		break;

	case DDERR_NOSTRETCHHW: 
		// The operation cannot be carried out because 
		// there is no hardware support for stretching. 
		lpszError = TEXT("DDERR_NOSTRETCHHW");
		break;

	case DDERR_NOT4BITCOLOR: 
		// The DirectDrawSurface object is not using a 
		// 4-bit color palette and the requested operation 
		// requires a 4-bit color palette. 
		lpszError = TEXT("DDERR_NOT4BITCOLOR");
		break;

	case DDERR_NOT4BITCOLORINDEX: 
		// The DirectDrawSurface object is not using a 4-bit 
		// color index palette and the requested operation 
		// requires a 4-bit color index palette. 
		lpszError = TEXT("DDERR_NOT4BITCOLORINDEX");
		break;

	case DDERR_NOT8BITCOLOR: 
		// The DirectDrawSurface object is not using an 8-bit 
		// color palette and the requested operation requires 
		// an 8-bit color palette. 
		lpszError = TEXT("DDERR_NOT8BITCOLOR");
		break;

	case DDERR_NOTAOVERLAYSURFACE: 
		// An overlay component is called for a non-overlay surface. 
		lpszError = TEXT("DDERR_NOTAOVERLAYSURFACE");
		break;

	case DDERR_NOTEXTUREHW: 
		// The operation cannot be carried out because no 
		// texture-mapping hardware is present or available. 
		lpszError = TEXT("DDERR_NOTEXTUREHW");
		break;

	case DDERR_NOTFLIPPABLE: 
		// An attempt has been made to flip a surface that cannot be flipped. 
		lpszError = TEXT("DDERR_NOTFLIPPABLE");
		break;

	case DDERR_NOTFOUND: 
		// The requested item was not found. 
		lpszError = TEXT("DDERR_NOTFOUND");
		break;

	case DDERR_NOTINITIALIZED: 
		// An attempt was made to call an interface method of a DirectDraw object 
		// created by CoCreateInstance before the object was initialized. 
		lpszError = TEXT("DDERR_NOTINITIALIZED");
		break;

	case DDERR_NOTLOCKED: 
		// An attempt is made to unlock a surface that was not locked. 
		lpszError = TEXT("DDERR_NOTLOCKED");
		break;

	case DDERR_NOTPAGELOCKED: 
		// An attempt is made to page unlock a surface 
		// with no outstanding page locks. 
		lpszError = TEXT("DDERR_NOTPAGELOCKED");
		break;

	case DDERR_NOTPALETTIZED: 
		// The surface being used is not a palette-based surface. 
		lpszError = TEXT("DDERR_NOTPALETTIZED");
		break;

	case DDERR_NOVSYNCHW: 
		// The operation cannot be carried out because 
		// there is no hardware support for vertical blank synchronized operations. 
		lpszError = TEXT("DDERR_NOVSYNCHW");
		break;

	case DDERR_NOZBUFFERHW: 
		// The operation to create a z-buffer in display memory 
		// or to perform a blit using a z-buffer cannot be carried 
		// out because there is no hardware support for z-buffers. 
		lpszError = TEXT("DDERR_NOZBUFFERHW");
		break;

	case DDERR_NOZOVERLAYHW: 
		// The overlay surfaces cannot be z-layered based 
		// on the z-order because the hardware does not 
		// support z-ordering of overlays. 
		lpszError = TEXT("DDERR_NOZOVERLAYHW");
		break;

	case DDERR_OUTOFCAPS: 
		// The hardware needed for the requested operation has already been allocated. 
		lpszError = TEXT("DDERR_OUTOFCAPS");
		break;

	case DDERR_OUTOFMEMORY: 
		// DirectDraw does not have enough memory to perform the operation. 
		lpszError = TEXT("DDERR_OUTOFMEMORY");
		break;

	case DDERR_OUTOFVIDEOMEMORY: 
		// DirectDraw does not have enough display memory to perform the operation. 
		lpszError = TEXT("DDERR_OUTOFVIDEOMEMORY");
		break;

	case DDERR_OVERLAYCANTCLIP: 
		// The hardware does not support clipped overlays. 
		lpszError = TEXT("DDERR_OVERLAYCANTCLIP");
		break;

	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: 
		// An attempt was made to have more than one color key active on an overlay. 
		lpszError = TEXT("DDERR_OVERLAYCOLORKEYONLYONEACTIVE");
		break;

	case DDERR_OVERLAYNOTVISIBLE: 
		// The IDirectDrawSurface2::GetOverlayPosition method is called on a hidden overlay. 
		lpszError = TEXT("DDERR_OVERLAYNOTVISIBLE");
		break;

	case DDERR_PALETTEBUSY: 
		// Access to this palette is refused 
		// because the palette is locked by another thread. 
		lpszError = TEXT("DDERR_PALETTEBUSY");
		break;

	case DDERR_PRIMARYSURFACEALREADYEXISTS: 
		// This process has already created a primary surface. 
		lpszError = TEXT("DDERR_PRIMARYSURFACEALREADYEXISTS");
		break;

	case DDERR_REGIONTOOSMALL: 
		// The region passed to the 
		// IDirectDrawClipper::GetClipList method is too small. 
		lpszError = TEXT("DDERR_REGIONTOOSMALL");
		break;

	case DDERR_SURFACEALREADYATTACHED: 
		// An attempt was made to attach a surface to 
		// another surface to which it is already attached. 
		lpszError = TEXT("DDERR_SURFACEALREADYATTACHED");
		break;

	case DDERR_SURFACEALREADYDEPENDENT: 
		// An attempt was made to make a surface a dependency 
		// of another surface to which it is already dependent. 
		lpszError = TEXT("DDERR_SURFACEALREADYDEPENDENT");
		break;

	case DDERR_SURFACEBUSY: 
		// Access to the surface is refused because the 
		// surface is locked by another thread. 
		lpszError = TEXT("DDERR_SURFACEBUSY");
		break;

	case DDERR_SURFACEISOBSCURED: 
		// Access to the surface is refused 
		// because the surface is obscured. 
		lpszError = TEXT("DDERR_SURFACEISOBSCURED");
		break;

	case DDERR_SURFACELOST: 
		// Access to the surface is refused because the 
		// surface memory is gone. The DirectDrawSurface 
		// object representing this surface should have 
		// the IDirectDrawSurface2::Restore method called on it. 
		lpszError = TEXT("DDERR_SURFACELOST");
		break;

	case DDERR_SURFACENOTATTACHED: 
		// The requested surface is not attached. 
		lpszError = TEXT("DDERR_SURFACENOTATTACHED");
		break;

	case DDERR_TOOBIGHEIGHT: 
		// The height requested by DirectDraw is too large. 
		lpszError = TEXT("DDERR_TOOBIGHEIGHT");
		break;

	case DDERR_TOOBIGSIZE: 
		// The size requested by DirectDraw is too large. 
		// However, the individual height and width are OK.
		lpszError = TEXT("DDERR_TOOBIGSIZE");
		break;

	case DDERR_TOOBIGWIDTH: 
		// The width requested by DirectDraw is too large. 
		lpszError = TEXT("DDERR_TOOBIGWIDTH");
		break;

	case DDERR_UNSUPPORTED: 
		// The operation is not supported. 
		lpszError = TEXT("DDERR_UNSUPPORTED");
		break;

	case DDERR_UNSUPPORTEDFORMAT: 
		// The FourCC format requested is not supported by DirectDraw. 
		lpszError = TEXT("DDERR_UNSUPPORTEDFORMAT");
		break;

	case DDERR_UNSUPPORTEDMASK: 
		// The bitmask in the pixel format requested is not supported by DirectDraw. 
		lpszError = TEXT("DDERR_UNSUPPORTEDMASK");
		break;

	case DDERR_UNSUPPORTEDMODE: 
		// The display is currently in an unsupported mode. 
		lpszError = TEXT("DDERR_UNSUPPORTEDMODE");
		break;

	case DDERR_VERTICALBLANKINPROGRESS: 
		// A vertical blank is in progress. 
		lpszError = TEXT("DDERR_VERTICALBLANKINPROGRESS");
		break;

	case DDERR_WASSTILLDRAWING: 
		// The previous blit operation that is transferring 
		// information to or from this surface is incomplete. 
		lpszError = TEXT("DDERR_WASSTILLDRAWING");
		break;

	case DDERR_WRONGMODE:
		// This surface cannot be restored because it was created in a different mode. 
		lpszError = TEXT("DDERR_WRONGMODE");
		break;

	case DDERR_XALIGN:
		// The provided rectangle was not horizontally aligned on a required boundary. 
		lpszError = TEXT("DDERR_XALIGN");
		break;

	case DDERR_VIDEONOTACTIVE:
		// The video port is not active
		lpszError = TEXT("DDERR_VIDEONOTACTIVE");
		break;

	default:
		// Unknown DD/D3D/App Error
		wsprintf (szMsg, "Unknown Error #%ld", (DWORD)(hResult & 0x0000FFFFL));
		lpszError = szMsg;
		break;
	}

	// Copy DD/D3D Error string to buff
	cLen = strlen (lpszError);
	if (cLen >= cchError)
	{
		cLen = cchError - 1;
	}

	if (cLen)
	{
		strncpy (lpszErrorBuff, lpszError, cLen);
		lpszErrorBuff[cLen] = 0;
	}

	return TRUE;
} // End GetDDErrorString

