package org.atire.android;

import java.util.ArrayList;

import org.atire.Indexer;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class MobileSearchActivity extends Activity implements OnClickListener {
	private TextView textViewFiles;
	private TextView textViewResult;
	private ArrayList<String> files;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Button button = (Button)findViewById(R.id.index_button);
        button.setOnClickListener(this);
        
        textViewFiles = (TextView)findViewById(R.id.textViewFiles);
        textViewResult = (TextView)findViewById(R.id.textViewResult);
        
    }

	@Override
	public void onClick(View v) {
		new Indexer().index("/sdcard/data/moby.txt");
	}
}