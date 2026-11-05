#include <JuceHeader.h>
#include <cmath>

class LA2AProcessor final : public juce::AudioProcessor {
public:
  LA2AProcessor() : AudioProcessor(BusesProperties().withInput("In", juce::AudioChannelSet::stereo(), true)
    .withOutput("Out", juce::AudioChannelSet::stereo(), true)),
    state(*this, nullptr, "STATE", {
      std::make_unique<juce::AudioParameterFloat>("peak","Peak Reduction",0.0f,100.0f,35.0f),
      std::make_unique<juce::AudioParameterFloat>("gain","Gain",0.0f,100.0f,40.0f),
      std::make_unique<juce::AudioParameterBool>("limit","Limit",false),
      std::make_unique<juce::AudioParameterBool>("bypass","Bypass",false)}) {}
  const juce::String getName() const override { return "Arm Nongmuang LA-2A"; }
  void prepareToPlay(double s,int) override { sr=s; env=0; }
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout& b) const override {
    auto c=b.getMainInputChannelSet(); return c==juce::AudioChannelSet::mono()||c==juce::AudioChannelSet::stereo();
  }
  void processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) override {
    juce::ScopedNoDenormals guard;
    const float peak=state.getRawParameterValue("peak")->load();
    const float gain=state.getRawParameterValue("gain")->load();
    const bool limit=state.getRawParameterValue("limit")->load()>0.5f;
    const bool bypass=state.getRawParameterValue("bypass")->load()>0.5f;
    const float threshold=juce::jmap(peak,0.0f,100.0f,0.95f,0.12f);
    const float makeup=juce::Decibels::decibelsToGain(juce::jmap(gain,0.0f,100.0f,-3.0f,18.0f));
    const float ratio=limit?10.0f:3.8f;
    const float a=std::exp(-1.0f/(0.012f*(float)sr)), r=std::exp(-1.0f/(0.32f*(float)sr));
    float gr=0;
    for(int i=0;i<b.getNumSamples();++i){
      float d=0; for(int c=0;c<b.getNumChannels();++c)d=juce::jmax(d,std::abs(b.getSample(c,i)));
      float k=d>env?a:r; env=k*env+(1-k)*d; float red=1;
      if(env>threshold){float over=juce::Decibels::gainToDecibels(env/threshold); float comp=over-over/ratio;
        red=juce::Decibels::decibelsToGain(-comp); gr=juce::jmax(gr,comp);}
      for(int c=0;c<b.getNumChannels();++c){float x=b.getSample(c,i); if(!bypass)x=std::tanh(x*red*makeup*1.04f); b.setSample(c,i,x);}
    } reduction.store(gr);
  }
  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor()const override{return true;} bool acceptsMidi()const override{return false;}
  bool producesMidi()const override{return false;} bool isMidiEffect()const override{return false;}
  double getTailLengthSeconds()const override{return .5;} int getNumPrograms()override{return 1;}
  int getCurrentProgram()override{return 0;} void setCurrentProgram(int)override{}
  const juce::String getProgramName(int)override{return{};} void changeProgramName(int,const juce::String&)override{}
  void getStateInformation(juce::MemoryBlock& d)override{auto x=state.copyState().createXml();copyXmlToBinary(*x,d);}
  void setStateInformation(const void* d,int n)override{std::unique_ptr<juce::XmlElement>x(getXmlFromBinary(d,n));if(x)state.replaceState(juce::ValueTree::fromXml(*x));}
  juce::AudioProcessorValueTreeState state; std::atomic<float> reduction{0};
private: double sr=44100; float env=0;
};
class LA2AEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit LA2AEditor(LA2AProcessor& p):AudioProcessorEditor(p),proc(p){
    setSize(900,520);
    auto knob=[](juce::Slider& s){s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,70,20);};
    knob(peak);knob(gain);addAndMakeVisible(peak);addAndMakeVisible(gain);addAndMakeVisible(limit);addAndMakeVisible(bypass);
    title.setText("LA-2A  |  TUBE COMPRESSOR",juce::dontSendNotification);title.setFont(juce::Font(juce::FontOptions(30,juce::Font::bold)));title.setColour(juce::Label::textColourId,juce::Colour(0xffa8201d));title.setJustificationType(juce::Justification::centred);addAndMakeVisible(title);
    peak.setName("Peak Reduction");gain.setName("Gain");
    peakA=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.state,"peak",peak);
    gainA=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.state,"gain",gain);
    limitA=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(proc.state,"limit",limit);
    bypassA=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(proc.state,"bypass",bypass);
    startTimerHz(20);
  }
  void paint(juce::Graphics& g)override{
    g.fillAll(juce::Colour(0xffb9b9b5));g.setColour(juce::Colour(0xffe5e1d6));g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8),12);
    g.setColour(juce::Colours::black);g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8),12,2);
    auto r=juce::Rectangle<float>(290,110,320,125);g.setColour(juce::Colour(0xff3a2b17));g.fillRoundedRectangle(r.expanded(8),8);
    g.setColour(juce::Colour(0xffffd98a));g.fillRoundedRectangle(r,5);
    g.setColour(juce::Colours::black);g.drawText("VU  /  GAIN REDUCTION",300,240,300,28,juce::Justification::centred);
    g.drawText("Arm Nongmuang  •  Mue Perm Studio",250,470,400,24,juce::Justification::centred);
    g.setColour(juce::Colour(0xffaa2020));g.drawLine(450,220,500,145,3);
    g.setColour(juce::Colours::darkgrey);g.fillRoundedRectangle(320,285,260,18,5);
    g.setColour(juce::Colour(0xffb58b2d));g.fillRoundedRectangle(320,285,juce::jlimit(0.0f,260.0f,proc.reduction.load()/24.0f*260.0f),18,5);
  }
  void resized()override{title.setBounds(160,25,580,55);peak.setBounds(60,250,220,190);gain.setBounds(620,250,220,190);limit.setBounds(710,120,130,30);bypass.setBounds(710,165,130,30);}
private:
  void timerCallback()override{repaint();}
  LA2AProcessor& proc;juce::Slider peak,gain;juce::ToggleButton limit{"LIMIT"},bypass{"BYPASS"};juce::Label title;
  using SA=juce::AudioProcessorValueTreeState::SliderAttachment;using BA=juce::AudioProcessorValueTreeState::ButtonAttachment;
  std::unique_ptr<SA>peakA,gainA;std::unique_ptr<BA>limitA,bypassA;
};
juce::AudioProcessorEditor* LA2AProcessor::createEditor(){return new LA2AEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new LA2AProcessor();}
