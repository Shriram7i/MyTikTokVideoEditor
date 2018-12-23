package com.whensunset.mytiktokvideoeditor;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.whensunset.mttvideoeditorsdk.A;

public class MainActivity extends AppCompatActivity {
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    A a = new A();
    
    // Example of a call to a native method
    TextView tv = (TextView) findViewById(R.id.sample_text);
    tv.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        MainActivity.this.startActivity(new Intent(MainActivity.this, GameActivity.class));
      }
    });
  }
  
}
