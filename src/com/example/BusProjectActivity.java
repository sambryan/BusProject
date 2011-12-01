package com.example;

import android.app.Activity;
import android.os.Bundle;
import android.widget.*;
import android.content.res.*;
import android.content.*;
import android.app.*;

public class BusProjectActivity extends TabActivity {
    /** Called when the activity is first created. */
	@Override
	 public void onCreate(Bundle savedInstanceState) {
		    super.onCreate(savedInstanceState);
		    setContentView(R.layout.main);

		    Resources res = getResources(); // Resource object to get Drawables
		    TabHost tabHost = getTabHost();  // The activity TabHost
		    TabHost.TabSpec spec;  // Resusable TabSpec for each tab
		    Intent intent;  // Reusable Intent for each tab

		    // Create an Intent to launch an Activity for the tab (to be reused)
		    intent = new Intent().setClass(this, HomeViewActivity.class);

		    // Initialize a TabSpec for each tab and add it to the TabHost
		    spec = tabHost.newTabSpec("artists").setIndicator("Artists",
		    		 			res.getDrawable(R.drawable.ic_tab_pages))
		                  .setContent(intent);
		    tabHost.addTab(spec);

		    // Do the same for the other tabs
		    intent = new Intent().setClass(this, MapViewActivity.class);
		    spec = tabHost.newTabSpec("albums").setIndicator("Albums",
		                      res.getDrawable(R.drawable.ic_tab_pages))
		                  .setContent(intent);
		    tabHost.addTab(spec);

		    intent = new Intent().setClass(this, SettingsViewActivity.class);
		    spec = tabHost.newTabSpec("songs").setIndicator("Songs",
		                      res.getDrawable(R.drawable.ic_tab_pages))
		                  .setContent(intent);
		    tabHost.addTab(spec);

		    tabHost.setCurrentTab(2);
		}
	}