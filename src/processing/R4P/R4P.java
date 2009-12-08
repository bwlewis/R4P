/*  
 *  This file is part of the R for Processing (R4P) library.
 *  Copyright (c) 2009 by Michael Kane and B. W. Lewis
 *
 *  R4P is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation. 
 *
 *  R4P is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

package R4P;

import java.io.*;
import java.util.*;

import java.awt.*;
import java.awt.event.*;
import java.util.Vector;

import org.rosuda.JRI.Rengine;
import org.rosuda.JRI.REXP;
import org.rosuda.JRI.RList;
import org.rosuda.JRI.RVector;
import org.rosuda.JRI.RMainLoopCallbacks;
import org.rosuda.JRI.RConsoleOutputStream;

import processing.core.*;

public class R4P implements RMainLoopCallbacks, KeyListener
{
  public Rengine re;
  PApplet parent;
  REXP processing, R4Pinstance;
  Frame RFrame;
  TextArea ROutput;
  Thread thread;
  TextField RInput;
  Vector RHistory;
  int j;

  public void rWriteConsole (Rengine re, String text, int oType)
  {
    ROutput.append(text);
  }

  public void rBusy (Rengine re, int w)
  {
    if (w != 0)
      {
	RFrame.setCursor (new Cursor (Cursor.WAIT_CURSOR));
	RInput.setCursor (new Cursor (Cursor.WAIT_CURSOR));
	ROutput.setCursor (new Cursor (Cursor.WAIT_CURSOR));
      }
    else
      {
	RFrame.setCursor (new Cursor (Cursor.DEFAULT_CURSOR));
	RInput.setCursor (new Cursor (Cursor.DEFAULT_CURSOR));
	ROutput.setCursor (new Cursor (Cursor.DEFAULT_CURSOR));
      }
  }

// We don't actually use the reader below, input is handled by the AWT.
  public String rReadConsole (Rengine re, String prompt, int addToHistory)
  {
    re.rniIdle ();
    try
    {
      BufferedReader br =
	new BufferedReader (new InputStreamReader (System.in));
      String s = br.readLine ();
      return (s == null || s.length () == 0) ? s : s + "\n";
    }
    catch (Exception e)
    {
      System.out.println ("jriReadConsole exception: " + e.getMessage ());
    }
    return null;
  }

  public void rShowMessage (Rengine re, String message)
  {
    System.out.println ("rShowMessage \"" + message + "\"");
  }

  public String rChooseFile (Rengine re, int newFile)
  {
    FileDialog fd =
      new FileDialog (new Frame (),
		      (newFile == 0) ? "Select a file" : "Select a new file",
		      (newFile == 0) ? FileDialog.LOAD : FileDialog.SAVE);
    fd.show ();
    String res = null;
    if (fd.getDirectory () != null)
      res = fd.getDirectory ();
    if (fd.getFile () != null)
      res = (res == null) ? fd.getFile () : (res + fd.getFile ());
    return res;
  }

  public void rFlushConsole (Rengine re)
  {
  }

  public void rLoadHistory (Rengine re, String filename)
  {
  }
  public void rSaveHistory (Rengine re, String filename)
  {
  }

// utility functions:

  public int[] mouse ()
  {
    int[] j = { parent.mouseX, parent.mouseY };
    return j;
  }

  public int[] dims ()
  {
    int[] j = { parent.width, parent.height };
    return j;
  }

  public R4P (PApplet parent, boolean showConsole)
  {
    this.parent = parent;
    parent.registerDispose (this);
    ROutput = new TextArea ("", 22, 80, TextArea.SCROLLBARS_BOTH);
    ROutput.setFont (new Font ("Monospaced", Font.PLAIN, 11));
    ROutput.setEditable (false);
    ROutput.setBackground (Color.white);
    RInput = new TextField ("", 80);
    RInput.setFont (new Font ("Monospaced", Font.PLAIN, 11));
    RInput.setEditable (true);
    RInput.setBackground (Color.white);
    RFrame = new Frame ();
    RFrame.setSize (620, 400);
    RFrame.setTitle ("R Console");

    RHistory = new Vector ();
    j = 0;

    GridBagLayout gb = new GridBagLayout ();
    GridBagConstraints c = new GridBagConstraints ();
    RFrame.setLayout (gb);

    c.anchor = GridBagConstraints.NORTHWEST;
    c.insets = new Insets (2, 2, 0, 2);
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1;
    c.weighty = 1;
    c.gridy = 0;
    c.gridx = 0;
    gb.setConstraints (ROutput, c);
    RFrame.add (ROutput);

    c.anchor = GridBagConstraints.SOUTH;
    c.insets = new Insets (2, 2, 2, 2);
    c.fill = GridBagConstraints.BOTH;
    c.gridy = 1;
    c.gridx = 0;
    c.weighty = 0;
    gb.setConstraints (RInput, c);
    RFrame.add (RInput);
    RFrame.pack ();
    RFrame.addWindowListener (new WindowAdapter ()
			      {
			      public void windowClosing (WindowEvent e)
			      {
			      RFrame.setVisible (false);}
			      });

    String[]args = {"--no-save", "--vanilla"};
    re = new Rengine (args, false, this);
    re.waitForR ();
    if (showConsole)
      {
	RFrame.setVisible (true);
	RInput.addKeyListener (this);
        setConsole();
      }
    re.eval ("require ('rJava', quietly=TRUE)");
    re.eval ("require ('R4P', quietly=TRUE)");
    this.processing = re.createRJavaRef (parent);
    re.assign ("PApplet", processing);
    this.R4Pinstance = re.createRJavaRef (this);
    re.assign ("R4P", R4Pinstance);
  }

  public void setConsole ()
  {
// NB We can't really intercept in/out streams as Processing uses them
// within the IDE. The only real dis-advantage is that the cheesy R console
// requires that the user explicitly cat results to stdout to see them.
//    System.setOut(new PrintStream(new RConsoleOutputStream(re, 0)));
//    System.setErr(new PrintStream(new RConsoleOutputStream(re, 1)));
  }

  public void dispose ()
  {
  }

  public void keyPressed (KeyEvent e)
  {
    int code = e.getKeyCode ();
    int k = RHistory.size () - 1;
    switch (code)
      {
      case (KeyEvent.VK_ENTER):
	String s = RInput.getText ();
	RInput.setText (null);
	RFrame.setCursor (new Cursor (Cursor.WAIT_CURSOR));
	RInput.setCursor (new Cursor (Cursor.WAIT_CURSOR));
	ROutput.setCursor (new Cursor (Cursor.WAIT_CURSOR));
	RInput.setVisible (true);
	RFrame.repaint ();
	try
	{
	  re.eval (s);
	  RHistory.addElement (s);
	}
	catch (Exception ex)
	{
	  System.err.println (ex.toString ());
	}
	RFrame.setCursor (new Cursor (Cursor.DEFAULT_CURSOR));
	RInput.setCursor (new Cursor (Cursor.DEFAULT_CURSOR));
	ROutput.setCursor (new Cursor (Cursor.DEFAULT_CURSOR));
	j = RHistory.size ();
	break;
      case (KeyEvent.VK_DOWN):
	j += 1;
	if (j > k)
	  {
	    j = k;
	  }
	else
	  {
	    if (j > -1)
	      RInput.setText ((String) RHistory.elementAt (j));
	  }
	break;
      case (KeyEvent.VK_UP):
	j -= 1;
	if (j < 0)
	  {
	    j = 0;
	  }
	if (j <= k)
	  RInput.setText ((String) RHistory.elementAt (j));
	break;
      }
  }

  public void keyReleased (KeyEvent e)
  {
  }
  public void keyTyped (KeyEvent e)
  {
  }

}
