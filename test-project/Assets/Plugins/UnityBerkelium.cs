using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class UnityBerkelium : MonoBehaviour {

    public int width = 1024;
    public int height = 1024;
	public string url = "http://www.youtube.com/watch?v=oHg5SJYRHA0";
	public bool interactive = true;
	public bool useTransparency = false;
	
	//newly added!
	bool haveID;
	int currentID;
	int num = 0;
	
	public int mouseX;
	public int mouseY;
	public bool mouseInput;
	
    private Texture2D m_Texture;
    private Color[] m_Pixels;
    private GCHandle m_PixelsHandle;
        
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_init();
    
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_destroy();
    
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_update(int windowID);
    
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern int Berkelium_Window_create(int windowID, IntPtr colors, int width, int height, string url);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_destroy(int windowID);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_mouseDown(int windowID, int button);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_mouseUp(int windowID, int button);

	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_mouseMove(int windowID, int x, int y);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_textEvent(int windowID, char c);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern void Berkelium_Window_keyEvent(int windowID, int key);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern int Berkelium_Window_rectWidth(int windowID);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern int Berkelium_Window_rectHeight(int windowID);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern int Berkelium_Window_rectTop(int windowID);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern int Berkelium_Window_rectLeft(int windowID);
	
	[DllImport ("UnityBerkeliumPlugin")]
	private static extern bool Berkelium_Window_isDirty(int windowID);


    void Start () {
		
		// Initialize Berkelium
		Berkelium_init();

 		if(useTransparency){ 
			
			m_Texture = new Texture2D (width, height, TextureFormat.ARGB32, false);	
			renderer.material.shader = Shader.Find ("Transparent/Cutout/Diffuse");
			renderer.material.mainTextureScale = new Vector2 (1,-1);
			
 		} else{
			
			m_Texture = new Texture2D (width, height, TextureFormat.RGB24, false);
			renderer.material.shader = Shader.Find ("Diffuse");
			renderer.material.mainTextureScale = new Vector2 (1,-1);
			
		}
		
        // Create the pixel array for the plugin to write into at startup    
        m_Pixels = m_Texture.GetPixels (0);
		
        // "pin" the array in memory, so we can pass direct pointer to it's data to the plugin,
        // without costly marshaling of array of structures.
        m_PixelsHandle = GCHandle.Alloc(m_Pixels, GCHandleType.Pinned);
		
		// Improve rendering at shallow angles
		m_Texture.filterMode = FilterMode.Trilinear;
		m_Texture.anisoLevel = 8;
		m_Texture.mipMapBias = -0.5f;

        // Assign texture to the renderer
        if (renderer)
            renderer.material.mainTexture = m_Texture;
		
        // or gui texture
        else if (GetComponent(typeof(GUITexture)))
        {
            GUITexture gui = GetComponent(typeof(GUITexture)) as GUITexture;
            gui.texture = m_Texture;
        }
        else
        {
            Debug.Log("Game object has no renderer or gui texture to assign the generated texture to!");
        }
		
		currentID = m_Texture.GetInstanceID();
		
		// Create new web window
		Berkelium_Window_create(currentID, m_PixelsHandle.AddrOfPinnedObject(), width,height, url);
	
		print("Created new web window: " + currentID);
    }
    
    void OnDisable() {
		// Destroy the web window
		//Berkelium_Window_destroy(m_Texture.GetInstanceID());
		
        // Free the pinned array handle.
        m_PixelsHandle.Free();
		
		// Destroy Berkelium
		Berkelium_destroy();
    }

	void setPixelLoopRect()
	{	
 		if(Berkelium_Window_isDirty(currentID))
		{ 
			//----------------------structure for SetPixels (x y, blockWidth, blockHeight, colors, 0)
			
			//set a block of only the pixels that have been changed
			m_Texture.SetPixels (
			Berkelium_Window_rectLeft(currentID),
			Berkelium_Window_rectTop(currentID),
			Berkelium_Window_rectWidth(currentID),
			Berkelium_Window_rectHeight(currentID),
			m_Pixels, 0); 
			
			m_Texture.Apply ();
 		} 
	 }
 
 
    // Now we can simply call UpdateTexture which gets routed directly into the plugin
    void Update () {
		// Update Berkelium
		Berkelium_update(currentID);
	
		// Apply the changed pixels
		// TODO Ask Berkelium for the dirty rectangle and only set those pixels
		//m_Texture.SetPixels (m_Pixels, 0);
		setPixelLoopRect();

		keyHandler();
    }
	
	
	void keyHandler()
	{
		if(Input.inputString!="")
		{
			char c = Input.inputString[0];
			string bs = Input.inputString;
			Berkelium_Window_textEvent(currentID, c);
			if (bs == "\b")
			{ 
				Berkelium_Window_keyEvent(currentID, 08);
			}
		}
	}
	
 	void OnMouseEnter()
	{
	}
	
	void OnMouseOver()
	{
		// Only when interactive is enabled
		if(interactive){
			RaycastHit hit;
			if (Physics.Raycast (Camera.main.ScreenPointToRay(Input.mousePosition), out hit)){
				
				int x =  (int) (hit.textureCoord.x * width);
				//int y = (int) (hit.textureCoord.y * height);
				int y = (int) Mathf.Abs(((hit.textureCoord.y * height)-height)); 
		
				Berkelium_Window_mouseMove(currentID, x, y);
			}
		}
	}
	
	void OnMouseExit()
	{
	}
	
	void OnMouseDown()
	{
		// Only when interactive is enabled
		if(interactive)
		{
			RaycastHit hit;
			if (Physics.Raycast (Camera.main.ScreenPointToRay(Input.mousePosition), out hit))
			{
				int x =  (int) (hit.textureCoord.x * width);
					//int y = (int) (hit.textureCoord.y * height);
					int y = (int) Mathf.Abs(((hit.textureCoord.y * height)-height)); 
		
				Berkelium_Window_mouseMove(currentID, x, y);
				Berkelium_Window_mouseDown(currentID, 0);
			}
		}
	}
	
	void OnMouseUp()
	{
		// Only when interactive is enabled
		if(interactive)
		{
			RaycastHit hit;
			if (Physics.Raycast (Camera.main.ScreenPointToRay(Input.mousePosition), out hit))
			{
				int x =  (int) (hit.textureCoord.x * width);
					//int y = (int) (hit.textureCoord.y * height);
					int y = (int) Mathf.Abs(((hit.textureCoord.y * height)-height)); 
		
				Berkelium_Window_mouseMove(currentID, x, y);
				Berkelium_Window_mouseUp(currentID, 0);
			}
		}
	}
	void OnGUI (){
		GUI.Label (new Rect (20,  0, 500, 20), "Current Window ID = " + currentID);
		GUI.Label (new Rect (20, 20, 500, 20), "Mouse input? = " + mouseInput);
		GUI.Label (new Rect (20, 40, 500, 20), "X = " + mouseX);
		GUI.Label (new Rect (20, 60, 500, 20), "Y = " + mouseY);
		GUI.Label (new Rect (20, 80, 500, 20), "num = " + num);
		//rect
 		GUI.Label (new Rect (20, 100, 500, 20), "rect left = " + Berkelium_Window_rectLeft(currentID));
		GUI.Label (new Rect (20, 120, 500, 20), "rect top = " + Berkelium_Window_rectTop(currentID));
		GUI.Label (new Rect (20, 140, 500, 20), "rect width = " + Berkelium_Window_rectWidth(currentID));
		GUI.Label (new Rect (20, 160, 500, 20), "rect height = " + Berkelium_Window_rectHeight(currentID)); 
		GUI.Label (new Rect (20, 180, 500, 20), "rect dirty? = " + Berkelium_Window_isDirty(currentID)); 
	}
}


