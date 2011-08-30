/*****************************************************************************
 * connectioncontainer.cpp: ActiveX control for VLC
 *****************************************************************************
 * Copyright (C) 2005 the VideoLAN team
 * Copyright (C) 2010 M2X BV
 *
 * Authors: Damien Fouilleul <Damien.Fouilleul@laposte.net>
 *          Jean-Paul Saman <jpsaman@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include "plugin.h"
#include "connectioncontainer.h"

#include "utils.h"

#include <assert.h>
#include <rpc.h>
#include <rpcndr.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_GUID(IID_IGlobalInterfaceTable,     0x00000146, 0x0000, 0x0000, 0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);
DEFINE_GUID(CLSID_StdGlobalInterfaceTable, 0x00000323, 0x0000, 0x0000, 0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);

const GUID  IID_IGlobalInterfaceTable = { 0x00000146, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46} };
const CLSID CLSID_StdGlobalInterfaceTable = { 0x00000323, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46} };
////////////////////////////////////////////////////////////////////////////////////////////////

/* this function object is used to return the value from a map pair */
struct VLCEnumConnectionsDereference
{
    CONNECTDATA operator()(const map<DWORD,LPUNKNOWN>::iterator& i)
    {
        CONNECTDATA cd;

        i->second->AddRef();

        cd.dwCookie = i->first;
        cd.pUnk     = i->second;
        return cd;
    };
};

class VLCEnumConnections : public VLCEnumIterator<IID_IEnumConnections,
    IEnumConnections,
    CONNECTDATA,
    map<DWORD,LPUNKNOWN>::iterator,
    VLCEnumConnectionsDereference>
{
public:
    VLCEnumConnections(map<DWORD,LPUNKNOWN> &m) :
        VLCEnumIterator<IID_IEnumConnections,
            IEnumConnections,
            CONNECTDATA,
            map<DWORD,LPUNKNOWN>::iterator,
            VLCEnumConnectionsDereference> (m.begin(), m.end())
    {};
};

////////////////////////////////////////////////////////////////////////////////////////////////

/* this function object is used to retain the dereferenced iterator value */
struct VLCEnumConnectionPointsDereference
{
    LPCONNECTIONPOINT operator()(const vector<LPCONNECTIONPOINT>::iterator& i)
    {
        LPCONNECTIONPOINT cp = *i;
        cp->AddRef();
        return cp;
    }
};

class VLCEnumConnectionPoints: public VLCEnumIterator<IID_IEnumConnectionPoints,
    IEnumConnectionPoints,
    LPCONNECTIONPOINT,
    vector<LPCONNECTIONPOINT>::iterator,
    VLCEnumConnectionPointsDereference>
{
public:
    VLCEnumConnectionPoints(vector<LPCONNECTIONPOINT>& v) :
        VLCEnumIterator<IID_IEnumConnectionPoints,
            IEnumConnectionPoints,
            LPCONNECTIONPOINT,
            vector<LPCONNECTIONPOINT>::iterator,
            VLCEnumConnectionPointsDereference> (v.begin(), v.end())
    {};
};

////////////////////////////////////////////////////////////////////////////////////////////////
// EventSystemProxyWnd
////////////////////////////////////////////////////////////////////////////////////////////////
class EventSystemProxyWnd
{
private:
    enum{
        WM_NEW_EVENT_NOTIFY = WM_USER+1
    };
    static LPCTSTR getClassName(void) { return TEXT("VLC ActiveX ESP Class"); };
    static void RegisterWndClassName();
    static void UnRegisterWndClassName();
    static LRESULT CALLBACK ESPWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static HINSTANCE _hInstance;
    static ATOM _ESP_wndclass_atom;
    HWND _hWnd;

public:
    static EventSystemProxyWnd* CreateESPWindow(HINSTANCE hInstance, VLCConnectionPointContainer *pCPC);

private:
    EventSystemProxyWnd(HWND hWnd, VLCConnectionPointContainer *pCPC)
        :_pCPC(pCPC), _HasUnprocessedNotify(false), _hWnd(hWnd){};
    void ProcessNotify();

public:
    void DestroyWindow()
        {::DestroyWindow(_hWnd);};

    //allowed to invoke from any thread
    void NewEventNotify();

private:
    VLCConnectionPointContainer *_pCPC;
    volatile bool _HasUnprocessedNotify;
};

HINSTANCE EventSystemProxyWnd::_hInstance=0;
ATOM EventSystemProxyWnd::_ESP_wndclass_atom=0;


void EventSystemProxyWnd::RegisterWndClassName()
{
    WNDCLASS wClass;
    memset(&wClass, 0, sizeof(WNDCLASS));

    if( ! GetClassInfo(_hInstance, getClassName(), &wClass) )
    {
        wClass.lpfnWndProc    = ESPWindowProc;
        wClass.hInstance      = _hInstance;
        wClass.lpszClassName  = getClassName();

        _ESP_wndclass_atom = RegisterClass(&wClass);
    }
    else
    {
        _ESP_wndclass_atom = 0;
    }
}

void EventSystemProxyWnd::UnRegisterWndClassName()
{
    if(0 != _ESP_wndclass_atom){
        UnregisterClass(MAKEINTATOM(_ESP_wndclass_atom), _hInstance);
        _ESP_wndclass_atom = 0;
    }
}

LRESULT CALLBACK EventSystemProxyWnd::ESPWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    EventSystemProxyWnd* ESP = reinterpret_cast<EventSystemProxyWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch( uMsg )
    {
        case WM_CREATE:{
            CREATESTRUCT* CreateStruct = (CREATESTRUCT*)(lParam);
            VLCConnectionPointContainer * pCPC = reinterpret_cast<VLCConnectionPointContainer *>(CreateStruct->lpCreateParams);

            ESP = new EventSystemProxyWnd(hWnd, pCPC);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ESP));
            break;
        }
        case WM_NCDESTROY:
            delete ESP;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
            break;
        case WM_NEW_EVENT_NOTIFY:
            ESP->ProcessNotify();
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

EventSystemProxyWnd* EventSystemProxyWnd::CreateESPWindow(HINSTANCE hInstance, VLCConnectionPointContainer *pCPC)
{
    //save hInstance for future use
    _hInstance = hInstance;

    RegisterWndClassName();

    HWND hWnd = CreateWindow(getClassName(),
                             0,
                             0,
                             0, 0, 0, 0,
                             0,
                             0,
                             _hInstance,
                             pCPC
                             );

    if(hWnd)
        return reinterpret_cast<EventSystemProxyWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    return 0;
}

void EventSystemProxyWnd::NewEventNotify()
{
    if(!_HasUnprocessedNotify){
        _HasUnprocessedNotify=true;
        PostMessage(_hWnd, WM_NEW_EVENT_NOTIFY, 0, 0);
    }
}

void EventSystemProxyWnd::ProcessNotify()
{
    while(_pCPC->isRunning){
        VLCDispatchEvent *ev = 0;
        EnterCriticalSection(&(_pCPC->csEvents));
        if(!_pCPC->_q_events.empty()){
            ev = _pCPC->_q_events.front();
            _pCPC->_q_events.pop();
        }
        LeaveCriticalSection(&(_pCPC->csEvents));

        if(ev){
            _pCPC->_p_events->fireEvent(ev->_dispId, &ev->_dispParams);
            delete ev;
        }
        else break;
    }
    _HasUnprocessedNotify=false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// VLCConnectionPoint
////////////////////////////////////////////////////////////////////////////////////////////////

VLCConnectionPoint::VLCConnectionPoint(IConnectionPointContainer *p_cpc, REFIID iid) :
        _iid(iid), _p_cpc(p_cpc)
{
    // Get the Global Interface Table per-process singleton:
    CoCreateInstance(CLSID_StdGlobalInterfaceTable, 0,
                          CLSCTX_INPROC_SERVER,
                          IID_IGlobalInterfaceTable,
                          reinterpret_cast<void**>(&m_pGIT));
};

VLCConnectionPoint::~VLCConnectionPoint()
{
    // Revoke interfaces from the GIT:
    map<DWORD,LPUNKNOWN>::iterator end = _connections.end();
    map<DWORD,LPUNKNOWN>::iterator iter = _connections.begin();

    while( iter != end )
    {
        m_pGIT->RevokeInterfaceFromGlobal((DWORD)iter->second);
        ++iter;
    }
    m_pGIT->Release();
};

STDMETHODIMP VLCConnectionPoint::GetConnectionInterface(IID *iid)
{
    if( NULL == iid )
        return E_POINTER;

    *iid = _iid;
    return S_OK;
};

STDMETHODIMP VLCConnectionPoint::GetConnectionPointContainer(LPCONNECTIONPOINTCONTAINER *ppCPC)
{
    if( NULL == ppCPC )
        return E_POINTER;

    _p_cpc->AddRef();
    *ppCPC = _p_cpc;
    return S_OK;
};

STDMETHODIMP VLCConnectionPoint::Advise(IUnknown *pUnk, DWORD *pdwCookie)
{
    static DWORD dwCookieCounter = 0;
    HRESULT hr;

    if( (NULL == pUnk) || (NULL == pdwCookie) )
        return E_POINTER;

    hr = pUnk->QueryInterface(_iid, (LPVOID *)&pUnk);
    if( SUCCEEDED(hr) )
    {
        DWORD dwGITCookie;
        hr = m_pGIT->RegisterInterfaceInGlobal( pUnk, _iid, &dwGITCookie );
        if( SUCCEEDED(hr) )
        {
            *pdwCookie = ++dwCookieCounter;
            _connections[*pdwCookie] = (LPUNKNOWN) dwGITCookie;
        }
        pUnk->Release();
    }
    return hr;
};

STDMETHODIMP VLCConnectionPoint::Unadvise(DWORD pdwCookie)
{
    map<DWORD,LPUNKNOWN>::iterator pcd = _connections.find((DWORD)pdwCookie);
    if( pcd != _connections.end() )
    {
        m_pGIT->RevokeInterfaceFromGlobal((DWORD)pcd->second);
        _connections.erase(pdwCookie);
        return S_OK;
    }
    return CONNECT_E_NOCONNECTION;
};

STDMETHODIMP VLCConnectionPoint::EnumConnections(IEnumConnections **ppEnum)
{
    if( NULL == ppEnum )
        return E_POINTER;

    *ppEnum = dynamic_cast<LPENUMCONNECTIONS>(new VLCEnumConnections(_connections));

    return (NULL != *ppEnum ) ? S_OK : E_OUTOFMEMORY;
};

void VLCConnectionPoint::fireEvent(DISPID dispId, DISPPARAMS *pDispParams)
{
    map<DWORD,LPUNKNOWN>::iterator end = _connections.end();
    map<DWORD,LPUNKNOWN>::iterator iter = _connections.begin();

    HRESULT hr = S_OK;

    while( iter != end )
    {
        DWORD dwCookie = (DWORD)iter->second;
        LPUNKNOWN pUnk;

        hr = m_pGIT->GetInterfaceFromGlobal( dwCookie, _iid,
                                             reinterpret_cast<void **>(&pUnk) );
        if( SUCCEEDED(hr) )
        {
            IDispatch *pDisp;
            hr = pUnk->QueryInterface(_iid, (LPVOID *)&pDisp);
            if( SUCCEEDED(hr) )
            {
                pDisp->Invoke(dispId, IID_NULL, LOCALE_USER_DEFAULT,
                              DISPATCH_METHOD, pDispParams, NULL, NULL, NULL);
                pDisp->Release();
            }
            pUnk->Release();
        }
        ++iter;
    }
};

void VLCConnectionPoint::firePropChangedEvent(DISPID dispId)
{
    map<DWORD,LPUNKNOWN>::iterator end = _connections.end();
    map<DWORD,LPUNKNOWN>::iterator iter = _connections.begin();

    while( iter != end )
    {
        LPUNKNOWN pUnk;
        HRESULT hr;

        hr = m_pGIT->GetInterfaceFromGlobal( (DWORD)iter->second, IID_IUnknown,
                                              reinterpret_cast<void **>(&pUnk) );
        if( SUCCEEDED(hr) )
        {
            IPropertyNotifySink *pPropSink;
            hr = pUnk->QueryInterface( IID_IPropertyNotifySink, (LPVOID *)&pPropSink );
            if( SUCCEEDED(hr) )
            {
                pPropSink->OnChanged(dispId);
                pPropSink->Release();
            }
            pUnk->Release();
        }
        ++iter;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////

VLCDispatchEvent::~VLCDispatchEvent()
{
    //clear event arguments
    if( NULL != _dispParams.rgvarg )
    {
        for(unsigned int c = 0; c < _dispParams.cArgs; ++c)
            VariantClear(_dispParams.rgvarg + c);
        CoTaskMemFree(_dispParams.rgvarg);
    }
    if( NULL != _dispParams.rgdispidNamedArgs )
        CoTaskMemFree(_dispParams.rgdispidNamedArgs);
};

////////////////////////////////////////////////////////////////////////////////////////////////
// VLCConnectionPointContainer
////////////////////////////////////////////////////////////////////////////////////////////////
extern HMODULE DllGetModule();
VLCConnectionPointContainer::VLCConnectionPointContainer(VLCPlugin *p_instance) :
    _p_instance(p_instance), freeze(FALSE), isRunning(TRUE), _ESProxyWnd(0)
{
    _p_events = new VLCConnectionPoint(dynamic_cast<LPCONNECTIONPOINTCONTAINER>(this),
            _p_instance->getDispEventID());

    _v_cps.push_back(dynamic_cast<LPCONNECTIONPOINT>(_p_events));

    _p_props = new VLCConnectionPoint(dynamic_cast<LPCONNECTIONPOINTCONTAINER>(this),
            IID_IPropertyNotifySink);

    _v_cps.push_back(dynamic_cast<LPCONNECTIONPOINT>(_p_props));

    // init protection
    InitializeCriticalSection(&csEvents);

    _ESProxyWnd = EventSystemProxyWnd::CreateESPWindow(DllGetModule(), this);
};

VLCConnectionPointContainer::~VLCConnectionPointContainer()
{
    isRunning = FALSE;
    freeze = TRUE;

    if(_ESProxyWnd)
        _ESProxyWnd->DestroyWindow();
    _ESProxyWnd=0;

    DeleteCriticalSection(&csEvents);

    _v_cps.clear();
    while(!_q_events.empty()) {
        delete _q_events.front();
        _q_events.pop();
    };

    delete _p_props;
    delete _p_events;
};

STDMETHODIMP VLCConnectionPointContainer::EnumConnectionPoints(LPENUMCONNECTIONPOINTS *ppEnum)
{
    if( NULL == ppEnum )
        return E_POINTER;

    *ppEnum = dynamic_cast<LPENUMCONNECTIONPOINTS>(new VLCEnumConnectionPoints(_v_cps));

    return (NULL != *ppEnum ) ? S_OK : E_OUTOFMEMORY;
};

STDMETHODIMP VLCConnectionPointContainer::FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP)
{
    if( NULL == ppCP )
        return E_POINTER;

    *ppCP = NULL;

    if( IID_IPropertyNotifySink == riid )
    {
        _p_props->AddRef();
        *ppCP = dynamic_cast<LPCONNECTIONPOINT>(_p_props);
    }
    else if( _p_instance->getDispEventID() == riid )
    {
        _p_events->AddRef();
        *ppCP = dynamic_cast<LPCONNECTIONPOINT>(_p_events);
    }
    else
        return CONNECT_E_NOCONNECTION;

    return NOERROR;
};

void VLCConnectionPointContainer::freezeEvents(BOOL bFreeze)
{
    EnterCriticalSection(&csEvents);
    freeze = bFreeze;
    LeaveCriticalSection(&csEvents);
};

void VLCConnectionPointContainer::fireEvent(DISPID dispId, DISPPARAMS* pDispParams)
{
    if(_ESProxyWnd){
        EnterCriticalSection(&csEvents);

        // queue event for later use when container is ready
        _q_events.push(new VLCDispatchEvent(dispId, *pDispParams));
        if( _q_events.size() > 1024 )
        {
            // too many events in queue, get rid of older one
            delete _q_events.front();
            _q_events.pop();
        }
        LeaveCriticalSection(&csEvents);
        _ESProxyWnd->NewEventNotify();
    }
};

void VLCConnectionPointContainer::firePropChangedEvent(DISPID dispId)
{
    if( ! freeze )
        _p_props->firePropChangedEvent(dispId);
};

